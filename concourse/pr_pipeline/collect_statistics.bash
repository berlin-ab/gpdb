#!/usr/bin/env bash

set -e
set -o nounset


collect_statistics() {
    echo "nproc: ";
    nproc;

    echo "";
    echo "free: ";
    free -m;
    
    echo "";
    echo "vmstat: ";
    vmstat --unit M;
    
    echo "";
    echo "uptime: ";
    uptime;
}


collect_periodically() {
    while true; do
	sleep $COLLECT_EVERY_N_SECONDS;
	collect_statistics
    done;
}


_main() {
    export -f collect_periodically
    export -f collect_statistics
    
    echo "Collecting statistics for $SECONDS_TO_RUN every $COLLECT_EVERY_N_SECONDS second(s)"
    
    timeout --foreground $SECONDS_TO_RUN \
	    bash -c "collect_periodically" || true

}


_main "$@"
