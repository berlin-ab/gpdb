-- assert data lives in master
select * from gp_segment_configuration;

-- assert no data exists on the segments
select * from gp_dist_random('gp_segment_configuration');