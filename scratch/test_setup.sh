#
# Ensure masterhost and standby host are configured in
# /etc/hosts and ~/.ssh/config. See ./scratch/sample_ssh_config
# and ./scratch/sample_etc_hosts for example configuration
#
# Ensure gpdb5 cluster has been stopped.
#

rm -rf /tmp/greenplum
mkdir -p /tmp/greenplum/{master,primary}

MACHINE_LIST_FILE=$PWD/scratch/hostfile gpinitsystem -c ./scratch/gpinitsystem_config -a

mkdir -p /tmp/greenplum/filespaces/{master,seg0,seg1,seg2}
gpfilespace --config "$PWD/scratch/gpfilespace_config" -h masterhost

gpinitstandby -a -s standbyhost -P 5001 -F pg_system:/tmp/greenplum/standby,foobar:/tmp/greenplum/filespaces/standby


