#!/usr/bin/env bash

docker run \
       -v ${PWD}:/gpdb_src \
       --workdir=/gpdb_src \
       --env MAKE_TASK='-C src/test/regress installcheck' \
       --rm pivotaldata/greenplum-server-ubuntu-18 \
       ./concourse/pr_pipeline/runner.bash
