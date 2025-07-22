#!/usr/bin/env python
import os
import sys

from build_utils import post_setup, pre_setup, set_up


lib_name = "SnoreCore"
addon_dir_name = "snore_core"

env = pre_setup(ARGUMENTS, Environment, Variables, Help, SConscript)

cpp_paths = []
sources = []

set_up(env, cpp_paths, sources, addon_dir_name, is_setup_for_self=True)

post_setup(env, cpp_paths, sources, lib_name, addon_dir_name, Default)
