-- cleanup
drop table example_table;
drop function find_leaf_partitions();
drop function find_leaves_with_root_oid();
drop function find_leaf_policies_with_root_policies();
drop function find_leaf_partitions_with_mismatching_policies_to_root(text);
drop function policy(regclass);
drop function find_distribution_for_table(regclass);

create schema myschema;
create schema myotherschema;
set search_path to myschema;


-- create a three-level partition
create table example_table (
	a int,
	b int,
	c int
)
distributed by (a)
partition by range (a)
subpartition by range (b)
subpartition by range (c)
(
	partition ein start (0) end (10) ( -- a
		subpartition partition_a start (0) end(5) ( -- b
			subpartition partition_c start (0) end (3), -- c
			subpartition partition_d start (3) end (5) -- c
		),
		subpartition partition_b start (5) end(10) ( -- b
			subpartition partition_e start(5) end (10) -- c
		)
	),
	partition zwei start (10) end (20) ( -- a
		subpartition partition_z start (10) end (20) ( -- b
			subpartition partition_y start (0) end (3) -- c
		)
	)
);

-- change the distribution policy of a leaf partition table
alter table example_table_1_prt_ein_2_prt_partition_a_3_prt_partition_c set distributed randomly;

-- output raw data
select oid, * from pg_partition;
select oid, * from pg_partition_rule;
select * from gp_distribution_policy;
select * from gp_distribution_policy where localoid IN (
	select parrelid from pg_partition
);


--------------

-- Library functions





create function find_leaf_partitions() returns table (leaf_table regclass, parent_tuple_oid oid) as $$
	select parchildrelid, paroid from pg_partition_rule where paroid in (
		select oid from pg_partition outer_pg_partition WHERE parlevel = (
			select max(parlevel) from pg_partition
			where pg_partition.parrelid = outer_pg_partition.parrelid
		)
	)
$$ language sql;


create function find_leaves_with_root_oid() returns table (parent_oid oid, leaf_table regclass) as $$
	select pg_partition.parrelid, leaves.leaf_table from find_leaf_partitions() as leaves
		inner join pg_partition on leaves.parent_tuple_oid = pg_partition.oid
$$ language sql;


create function find_leaf_policies_with_root_policies() returns table (
	parent_oid oid, leaf_table regclass, root_localoid oid, root_attrnums smallint[], leaf_localoid oid, leaf_attrnums smallint[]
	) as $$
	select * from find_leaves_with_root_oid() as leaves
		inner join gp_distribution_policy parent_policy on parent_policy.localoid = leaves.parent_oid
		inner join gp_distribution_policy child_policy on child_policy.localoid = leaves.leaf_table
$$ language sql;



create function find_leaf_partitions_with_mismatching_policies_to_root(schema_name text) returns table(
	leaf_table regclass,
	root_table_oid oid,
	root_table regclass
) as $$
	select leaf_table, root_localoid, root_localoid from
		find_leaf_policies_with_root_policies() as all_leaves
		inner join pg_namespace on pg_namespace.nspname = $1
		inner join pg_class on (pg_class.oid = leaf_table and pg_class.relnamespace = pg_namespace.oid)
		where coalesce(all_leaves.root_attrnums, cast('{}' as smallint[])) !=
			coalesce(all_leaves.leaf_attrnums, cast('{}' as smallint[]));
$$ language sql;



create function policy (table_oid regclass) returns table(attnum smallint) as $$
	select unnest(attrnums) from gp_distribution_policy where localoid = $1;
$$ language sql;


create function find_distribution_for_table(some_table regclass) returns name[] as $$
	select array_agg(attname order by row_number) attributes
		from pg_attribute join (
			select attnum, row_number() over() from policy($1)
		) t(attnum, row_number) on pg_attribute.attnum = t.attnum
		where attrelid = $1;
$$ language sql;


-- expect nothing to show up
select * from find_leaf_partitions_with_mismatching_policies_to_root('myotherschema')
	where find_distribution_for_table(leaf_table)
		IS DISTINCT FROM find_distribution_for_table(root_table_oid);


-- expect example_table_1_prt_ein_2_prt_partition_a_3_prt_partition_c to show up
select * from find_leaf_partitions_with_mismatching_policies_to_root('myschema')
	where find_distribution_for_table(leaf_table)
		IS DISTINCT FROM find_distribution_for_table(root_table_oid);

