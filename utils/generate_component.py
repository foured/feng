import os

script_dir = os.path.dirname(os.path.abspath(__file__))
to_save_dir = os.path.join(script_dir, '../feng/src/logic/world/components/')

component_name = input("Enter component class name: ")

with open(os.path.join(script_dir, 'templates/component_h.txt')) as dH_file:
    dH = dH_file.read().replace('__CLASS_NAME__', component_name)
    with open(to_save_dir + component_name + '.h', 'w') as ndH:
        ndH.write(dH)
with open(os.path.join(script_dir, 'templates/component_cpp.txt')) as dCPP_file:
    dCPP = dCPP_file.read().replace('__CLASS_NAME__', component_name)
    with open(to_save_dir + component_name + '.cpp', 'w') as ndCPP:
        ndCPP.write(dCPP)