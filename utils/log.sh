#!/bin/bash
# file: log.sh
# desc: A simple log API for bshell

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
BLUE='\033[0;34m'
NC='\033[0m'

LOG_LEVEL_DEBUG=0
LOG_LEVEL_INFO=1
LOG_LEVEL_WARN=2
LOG_LEVEL_ERROR=3

CURRENT_LOG_LEVEL=$LOG_LEVEL_INFO

function log() {
  local level="$1"
  local message="$2"
  local timestamp=$(date '+%Y-%m-%d %H:%M:%S')

  case $level in
    $LOG_LEVEL_DEBUG)
      echo -e "${BLUE}[$timestamp] [DEBUG] $message${NC}"
      ;;
    $LOG_LEVEL_INFO)
      echo -e "${GREEN}[$timestamp] [INFO] $message${NC}"
      ;;
    $LOG_LEVEL_WARN)
      echo -e "${YELLOW}[$timestamp] [WARN] $message${NC}"
      ;;
    $LOG_LEVEL_ERROR)
      echo -e "${RED}[$timestamp] [ERROR] $message${NC}"
      ;;
    *)
      echo -e "[$timestamp] $message"
      ;;
  esac
}
