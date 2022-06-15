import xml.dom.minidom as minidom
import xml.dom
import os
import sys

if len(sys.argv) != 3:
    print("Error parameter!")
    print("Format: python3 mk_materializer_cfg.py [bml path] [output folder]")
    sys.exit(1)

bml_path = os.path.abspath(sys.argv[1])
output_path = os.path.abspath(sys.argv[2])
if not (output_path.endswith("/") or output_path.endswith("\\")):
    output_path += '\\'

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

write_macro(dom, node_property_group, node_item_group, 'YYCMOD_OUTPUT', output_path)
write_macro(dom, node_property_group, node_item_group, 'BMLDEV_PATH', bml_path)

with open('MacroProp.props', 'w', encoding='utf-8') as f:
    dom.writexml(f, addindent='\t', newl='\n', encoding='utf-8')
