import xml.dom.minidom as minidom
import xml.dom
import os, sys, argparse

# Analyze command

def abs_path_collection(path_collection):
    return ';'.join(
        map(lambda x: os.path.abspath(x), path_collection.split(';'))
    )

parser = argparse.ArgumentParser(description='MSVC Props Generator')
parser.add_argument('-t', '--bml-type', required=True, action='store', dest='bml_type', choices=('BML', 'BMLP'))
parser.add_argument('-p', '--output-path', required=True, action='store', dest='output_path')
parser.add_argument('-s', '--output-suffix', required=True, action='store', dest='output_suffix')
parser.add_argument('-i', '--bml-include-path', required=True, action='store', dest='bml_include_path')
parser.add_argument('-l', '--bml-lib-path', required=True, action='store', dest='bml_lib_path')
parser.add_argument('-n', '--bml-lib-name', required=True, action='store', dest='bml_lib_name')

args = parser.parse_args()

bml_type = 'YYCMOD_' + args.bml_type + '_USED'
bml_include_path = abs_path_collection(args.bml_include_path)
bml_lib_path = abs_path_collection(args.bml_lib_path)
bml_lib_name = args.bml_lib_name
output_path = os.path.abspath(args.output_path)
if not (output_path.endswith("/") or output_path.endswith("\\")):
    output_path += '\\'
output_suffix = args.output_suffix
if not output_suffix.startswith('.'):
    output_suffix = '.' + output_suffix

# Write prop files

def write_macro(dom, node_property_group, node_item_group, macro_upcase, data):
    node = dom.createElement(macro_upcase)
    if data != '':
        node.appendChild(dom.createTextNode(data))
    node_property_group.appendChild(node)

    node = dom.createElement("BuildMacro")
    node.setAttribute('Include', macro_upcase)
    inter_node = dom.createElement('Value')
    inter_node.appendChild(dom.createTextNode("$({})".format(macro_upcase)))
    node.appendChild(inter_node)
    node_item_group.appendChild(node)

dom = minidom.getDOMImplementation().createDocument(None, 'Project', None)
root = dom.documentElement
root.setAttribute('ToolsVersion', '4.0')
root.setAttribute('xmlns', 'http://schemas.microsoft.com/developer/msbuild/2003')

cache = dom.createElement('ImportGroup')
cache.setAttribute('Label', 'PropertySheets')
root.appendChild(cache)

node_property_group = dom.createElement('PropertyGroup')
node_property_group.setAttribute('Label', 'UserMacros')
root.appendChild(node_property_group)
node_item_group = dom.createElement('ItemGroup')
root.appendChild(node_item_group)

write_macro(dom, node_property_group, node_item_group, 'YYCMOD_OUTPUT_PATH', output_path)
write_macro(dom, node_property_group, node_item_group, 'YYCMOD_OUTPUT_SUFFIX', output_suffix)
write_macro(dom, node_property_group, node_item_group, 'BML_TYPE', bml_type)
write_macro(dom, node_property_group, node_item_group, 'BML_INCLUDE_PATH', bml_include_path)
write_macro(dom, node_property_group, node_item_group, 'BML_LIB_PATH', bml_lib_path)
write_macro(dom, node_property_group, node_item_group, 'BML_LIB_NAME', bml_lib_name)
# solve shitty string onvertion error for BMLP
write_macro(dom, node_property_group, node_item_group, 'BML_EXTRA_OPTIONS', '/Zc:strictStrings-' if args.bml_type == 'BMLP' else '')

with open('MacroProp.props', 'w', encoding='utf-8') as f:
    dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')
