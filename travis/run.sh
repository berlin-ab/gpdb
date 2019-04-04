#!/usr/bin/env bash

export MAKE_TASK='-C src/test/regress installcheck'
docker run -v ${PWD}:/gpdb_src --workdir=/gpdb_src --rm pivotaldata/greenplum-server-ubuntu-18 ./concourse/pr_pipeline/runner.bash
