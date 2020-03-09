#!/usr/bin/env bash

. lib/gp_bash_functions.sh

__cleanupNumericUser() {
  dropuser $USER_NAME
}

it_should_quote_the_username_during_alter_user_in_SET_GP_USER_PW() {
  # mimic gpinitsystem setup
  GET_MASTER_PORT "$MASTER_DATA_DIRECTORY"
  EXIT_STATUS=0

  # given a user that is only a number
  USER_NAME=123456
  createuser $USER_NAME
  trap __cleanupNumericUser EXIT

  # when we run set user password
  SET_GP_USER_PW

  # then it should succeed
  if [ "$EXIT_STATUS" != "0" ]; then
    local error_message="$(tail -n 10 "$LOG_FILE")"
    echo "got an exit status of $EXIT_STATUS while running SET_GP_USER_PW for $USER_NAME, wanted success: $error_message"
    exit 1
  fi
}

main() {
  it_should_quote_the_username_during_alter_user_in_SET_GP_USER_PW
}

main
