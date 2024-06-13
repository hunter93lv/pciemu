# directories.mk - Makefile helper file for setting default directories
#
# Copyright (c) 2023 Luiz Henrique Suraty Filho <luiz-dev@suraty.com>
#
# SPDX-License-Identifier: GPL-2.0
#

root_dir = $(shell git rev-parse --show-toplevel)
include_dir := $(root_dir)/include
src_dir := $(root_dir)/src

qemu_dir := $(root_dir)/qemu
qemu_build_include_dir := $(qemu_dir)/build
qemu_include_dir := $(qemu_dir)/include

build_dir := build

