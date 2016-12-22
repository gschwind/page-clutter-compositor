#!/usr/bin/env python3
#-*- coding: utf-8 -*-

import re, os, sys
import xml.etree.ElementTree as ET
import argparse

r_invalid = re.compile('(\W+)')

interface_blacklist = set(['wl_display', 'wl_registry', 'wl_callback'])

#parser = argparse.ArgumentParser(description='Generate wayland API C++ to C wrapper')
#parser.add_argument('integers', metavar='N', type=int, nargs='+',
#        help='an integer for the accumulator')
#parser.add_argument('--sum', dest='accumulate', action='store_const',
#        const=sum, default=max,
#        help='sum the integers (default: find the max)')
#
#    args = parser.parse_args()
#print(args.accumulate(args.integers))

def gen_args_with_type(args, args_base = None):
 if args_base is None:
  args_base = list()
 arglist = args_base
 for arg in args:
  aname = arg.attrib['name']
  atype = arg.attrib['type']
  if atype != 'object':
   arglist.append('{0} {1}'.format(maptype[atype], aname))
  else:
   #arglist.append('struct {0} * {1}'.format(arg.attrib['interface'], aname))
   arglist.append('struct wl_resource * {1}'.format(arg.attrib['interface'], aname))
 return ', '.join(arglist)

def gen_args(args, args_base = None):
 if args_base is None:
  args_base = list()
 arglist = args_base
 for arg in args:
  aname = arg.attrib['name']
  arglist.append(aname)
 return ', '.join(arglist)


maptype = {
 'uint': 'uint32_t',
 'int': 'int32_t',
 'new_id': 'uint32_t',
 'string': 'const char *',
 'array': 'struct wl_array *',
 'fixed': 'wl_fixed_t',
 'fd': 'int',
 'object': None
}

def gen_header(fi_name, fo):
 fi_xname = os.path.basename(fi_name)[:-4]
 fi_uname = r_invalid.sub('_', fi_xname).upper()
 tree = ET.parse(fi_name)
 root = tree.getroot()
 fo.write("/* Generated by wayland-cxx-scanner.py */\n")
 fo.write("""
/*
 * Copyright (2016) Benoit Gschwind
 *
 * This file is part of page-compositor.
 *
 * page-compositor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * page-compositor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with page-compositor.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
""")
 
 fo.write("""
#ifndef WCXX_{UNAME}_HXX_
#define WCXX_{UNAME}_HXX_

#include <wayland-server-core.h>

namespace wcxx {{
""".format(UNAME = fi_uname))
 
 for interface in root.findall('interface'):
  interface_name = interface.attrib['name']
  if interface_name in interface_blacklist:
   continue
  fo.write("struct {0}_vtable {{\n".format(interface_name))
  fo.write('\tstruct wl_resource * _self_resource;\n\n'.format(interface_name))
  fo.write('\t{0}_vtable(struct wl_client *client, uint32_t version, uint32_t id);\n'.format(interface_name))
  fo.write("\tvirtual ~{0}_vtable() = default;\n".format(interface_name))
  funclist = []
  if len(interface.findall('event')) != 0:
   fo.write('\n\t/* events */\n')
  for event in interface.findall('event'):
   args = event.findall('arg')
   fo.write('\tvoid send_{1}({2});\n'.format(interface_name, event.attrib['name'], gen_args_with_type(args)))
  if len(interface.findall('request')) != 0:
   fo.write('\t\n\t/* requests */\n')
  for request in interface.findall('request'):
   args = request.findall('arg')
   fo.write('\tvirtual void recv_{1}({2}) = 0;\n'.format(interface_name, request.attrib['name'], gen_args_with_type(args, ['struct wl_client * client', 'struct wl_resource * resource'])))
  fo.write('\n\t/* called when libwayland is destroying the resource */\n')
  fo.write('\tvirtual void delete_resource(struct wl_resource * resource) = 0;\n'.format(interface_name))
  fo.write('};\n\n')
 fo.write("}}\n#endif /* WCXX_{UNAME}_HXX_ */\n".format(UNAME = fi_uname))

def gen_impl(fi_name, fo):
 fi_xname = os.path.basename(fi_name)[:-4]
 fi_uname = r_invalid.sub('_', fi_xname).upper()
 tree = ET.parse(fi_name)
 root = tree.getroot()
 fo.write("/* Generated by wayland-cxx-scanner.py */\n")
 fo.write("""
/*
 * Copyright (2016) Benoit Gschwind
 *
 * This file is part of page-compositor.
 *
 * page-compositor is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * page-compositor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with page-compositor.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "{XNAME}-interface.hxx"
#include "{XNAME}-server-protocol.h"

namespace wcxx {{

namespace hidden {{
""".format(XNAME=fi_xname))
 
 for interface in root.findall('interface'):
  interface_name = interface.attrib['name']
  if interface_name in interface_blacklist:
   continue
  fo.write('inline {0}_vtable * {0}_get(struct wl_resource * resource) {{\n'.format(interface_name))
  fo.write('\treturn reinterpret_cast<{0}_vtable *>(wl_resource_get_user_data(resource));\n'.format(interface_name))
  fo.write('}\n\n')
  for request in interface.findall('request'):
   args = request.findall('arg')
   args_with_type = gen_args_with_type(args, ['struct wl_client * client', 'struct wl_resource * resource'])
   args_no_type = gen_args(args, ['client', 'resource'])
   fo.write('void {0}_{1}({2}) {{\n'.format(interface_name, request.attrib['name'], args_with_type))
   fo.write('\t{0}_get(resource)->{0}_{1}({2});\n'.format(interface_name, request.attrib['name'], args_no_type))
   fo.write('}\n\n')
  fo.write('void {0}_delete_resource(struct wl_resource * resource) {{\n'.format(interface_name))
  fo.write('\t{0}_get(resource)->{0}_delete_resource(resource);\n'.format(interface_name))
  fo.write('}\n\n')
  fo.write('static struct {0}_interface const {0}_implementation = {{\n'.format(interface_name))
  requests = interface.findall('request')
  for request in requests[:-1]:
   fo.write('\t{0}_{1},\n'.format(interface_name, request.attrib['name']))
  if len(requests) > 0:
   fo.write('\t{0}_{1}\n'.format(interface_name, requests[-1].attrib['name']))
  fo.write('};\n\n')
 fo.write('}\n')
 
 for interface in root.findall('interface'):
  interface_name = interface.attrib['name']
  if interface_name in interface_blacklist:
   continue
  for event in interface.findall('event'):
   args = event.findall('arg')
   args_with_type = gen_args_with_type(args)
   args_no_type = gen_args(args, ['_self_resource'])
   fo.write('void {0}_vtable::send_{1}({2}) {{\n'.format(interface_name, event.attrib['name'], args_with_type))
   fo.write('\t{0}_send_{1}({2});\n'.format(interface_name, event.attrib['name'], args_no_type))
   fo.write('}\n\n')
  fo.write("""
void {0}_vtable::{0}_vtable(struct wl_client *client, uint32_t version, uint32_t id)
{{
_self_resource = wl_resource_create(client, &xdg_shell_interface, version, id);
wl_resource_set_implementation(_self_resource, &hidden::{0}_implementation, this, &hidden::{0}_delete_resource);
}}
""".format(interface_name))
 fo.write('}\n')
 
fi_name = sys.argv[2]
fi_xname = os.path.basename(fi_name)[:-4]

if sys.argv[1] == 'header':
 gen_header(fi_name, sys.stdout)
else:
 gen_impl(fi_name, sys.stdout)


