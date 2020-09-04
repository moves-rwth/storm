# -*- coding: UTF-8 -*-
"""
v0.5.16
New structure 
The max_rules_run number is read from analysis file
boolFailureState read too from analysis file 
+ correcting some bugs
"""

import re
import sys
import os
import numpy as np
import xml.etree.ElementTree as ET


if __name__ == "__main__":  

    if len(sys.argv) == 1:
        print("The program is launched with no argument")
        input_file_path = "export_fig0_r7.fi"
        input_analysis_path = "analysis_r7.xml"
        cpp_file_path = "FigaroModel.cpp"
        header_file_path = "FigaroModel.h"
    elif len(sys.argv) == 2:  # the program was launched with four arguments
        print("The program is launched with directory path")
        input_file_path = os.path.join(sys.argv[1].rstrip(),'export_fig0.fi')
        print(input_file_path)
        input_analysis_path = os.path.join(sys.argv[1].rstrip(),'analysis.xml')
        cpp_file_path = os.path.join(sys.argv[1].rstrip(), 'FigaroModel.cpp')
        header_file_path = os.path.join(sys.argv[1].rstrip(), 'FigaroModel.h')
    elif len(sys.argv) == 4:  # the program was launched with four arguments
        print("The program is launched with four arguments")
        input_file_path = sys.argv[1]
        input_analysis_path = sys.argv[2]
        cpp_file_path = sys.argv[3]
        header_file_path = sys.argv[4]
    else:
        print(len(sys.argv[1]))
        print(print(sys.argv[1]))
        exit("The program is launched without directory path")
        

print(input_file_path)

figaro_objects_pattern = re.compile("OBJECT\s*(.*?)\s*IS_A\s*(.*?);\s*(.*?)(?=\sOBJECT|\Z)",
                                    flags=re.DOTALL|re.UNICODE)

# adding of all the keywords
keywords = "INTERFACE|CONSTANT|ATTRIBUTE|INTERACTION|OCCURRENCE"

figaro_members_pattern = re.compile("({0})(.*?)(?={0}|\Z)".format(keywords),
                                    flags=re.DOTALL|re.UNICODE)
figaro_member_declaration_pattern = re.compile("\s*(.*?)\s*=\s*(.*?)\s*;",
                                               flags=re.DOTALL|re.UNICODE)

figaro_constant_declaration_pattern = re.compile("\s*(.*?)\s*DOMAIN\s*(.*?)\s*=\s*(.*?)\s*;",
                                               flags=re.DOTALL|re.UNICODE)

def remove_specials(string):
    """
    This function is used to remove line breaks in order to avoid some
    specific cases during the parsing.

    Parameters
    ----------
    string : TYPE
        DESCRIPTION.

    Returns
    -------
    string : TYPE
        DESCRIPTION.

    """
    string = string.replace('\n','')
    string = string.replace('\t','')
    return string

def change_boolean(string):
    string = string.replace('FALSE','false')
    string = string.replace('TRUE','true')
    return string


def identifier_general(string, selected_keywords, space_in_keywords = False):
    """
    Make it easier to parse a complex expression by generating the regular 
    expression from a list of keywords.

    Parameters
    ----------
    string : TYPE
        DESCRIPTION.
    selected_keywords : TYPE
        DESCRIPTION.
    space_in_keywords : TYPE, optional
        DESCRIPTION. The default is False.

    Returns
    -------
    TYPE
        DESCRIPTION.

    """
    # ----
    # v0.2 this version is more robust: taking into account the blank space
    # after the keywords eg: THEN while variable THEN_1 exists
    # ----
    if space_in_keywords == True:
        all_keywords = re.findall( r"([A-Z]+(?:(?!\s?[A-Z][a-z])\s?[A-Z])+)", string)
    all_keywords = re.findall(r"([A-Z]+(?:(?!\s?[A-Z][a-z])[A-Z])+)", string) 
    # print('all_kw : '+str(all_keywords))
    
    
    
    base = '(.*?)\s*'
        
    my_regex = '\s*'+base
    for keyword in all_keywords:
        # print("***" + keyword)
        if keyword in selected_keywords:
            my_regex += '(?='+keyword+'\\b)'+base
    my_regex+=';'
    # print("=> REGEX: "+my_regex)
    
    pattern = re.compile(my_regex, flags=re.DOTALL|re.UNICODE) 
    # print(all_keywords)
    # print('-----> ', my_regex)
    return pattern.findall(string)


def replaceRobust(my_str, keyword, replacement):
    """
    Allow to replace keywords when preceded by ' ' and followed by a blank 
    marker such as '\n', '\t', ...

    Parameters
    ----------
    my_str : string
        input string    
    keyword : string
        keyword to be replaced.
    str2 : string
        replacement of the keyword.

    Returns
    -------
    my_str : string
        input with robustly replaced keyword.

    """
    blanks = ['\n\t\t  ',' ','\n','\t']
    for b1 in blanks:
        for b2 in blanks:
            my_str = my_str.replace(b1+keyword+b2, replacement)
    return my_str



input_file = open(input_file_path, "r") 

figaro_code = input_file.read()
# figaro_code = figaro_code.replace(' OF ','_OF_')
figaro_code = replaceRobust(figaro_code, 'OF', '_OF_')
figaro_code = figaro_code.replace('<--',' = ')

keywords_attribute = ['LABEL', 'DOMAIN', 'REINITIALISATION', ";"]
keywords_interaction = ['GROUP','IF','STEP','THEN']



#figaro_occurrence_pattern = re.compile('\s*(.*?)\s*GROUP\s*(.*?)\s*IF\s*(.*?)\s*MAY_OCCUR\s*(.*?)\s*;',flags=re.DOTALL|re.UNICODE)
figaro_occurrence_pattern = re.compile('\s*(.*?)\s*\s*(.*?)\s*IF\s*(.*?)\s*MAY_OCCUR\s*(.*?)\s*;',flags=re.DOTALL|re.UNICODE)

figaro_may_occur_pattern = re.compile('(?=FAULT|REPAIR)\s*(.*?)\s*DIST\s*(.*?)\s*INDUCING\s*(.*?)\s*\Z')
figaro_INS_pattern = re.compile("(?=FAULT|TRANSITION)\s*(.*?)\s*(?=LABEL|\s)\s*(.*?)\s*(?=DIST)\s*(.*?)\s*(?=INDUCING)\s*(.*?)\s*\Z")
# keywords_occurrence = ['GROUP', 'IF', 'MAY OCCUR']
# keywords_may_occur = ['(?=FAULT|REPAIR)', 'LABEL']


currentIndentation = ""

all_constants = []
attributes_found = []
cpp_all_fire_occurrences = ""

## Handling of plurals steps for interactions
step_names = ["default_step"]
cpp_interractions_separated_steps = [[]]
declared_steps = re.findall(r"STEPS_ORDER\s*(.*?)\s*GROUP_NAMES", figaro_code,flags=re.DOTALL|re.UNICODE)
if len(declared_steps) > 0:
    declared_steps = declared_steps[0]
    declared_steps = declared_steps.split(";")
    declared_steps = [st.replace('\n','').replace('\t','').replace(' ','') for st in declared_steps]
else:
    declared_steps = step_names


cpp_all_attributes = "\n" + currentIndentation + "/* ---------- DECLARATION OF ATTRIBUTES ------------ */\n"
cpp_all_constants = "\n" + currentIndentation + "/* ---------- DECLARATION OF CONSTANTS ------------ */\n"
cpp_all_interactions = "\n" + currentIndentation + "/* ---------- DECLARATION OF INTERACTION RULES ------------ */\n"
cpp_all_occurrences = "\n" + currentIndentation + "/* ---------- DECLARATION OF OCCURRENCE RULES------------ */\n"
cpp_all_fire_occurrences = "\n" + currentIndentation + "\t/* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */\n"
cpp_declaration_attributes = "\n/* ---------- DECLARATION OF ATTRIBUTES ------------ */\n"

cpp_all_reinitialisations = ""

cpp_top_event = "" 

fire_occurrence_list = []
occurrence_count = 0
occurrence_sub_count = 1
ins_total_count = 0
instantaneous_variables = []
cpp_ins_occurrences = ""

enum_list = []

"""
===============================================================================
"""


        

currentIndentation += "\t";

objects_matches = np.array(figaro_objects_pattern.findall(figaro_code))    


om_count = 0
tmp_loop_count = 0
om_wrong_cases = []
for om in objects_matches[:,2]:    
    if om.find('OBJECT ', ) >=0 and om.find('OBJECT ', ) <= 5:
        # print("\n===================\n",objects_matches[tmp_loop_count,2])
        om_wrong_cases.append(tmp_loop_count)
        om_count += 1
        om_tmp = om
        objects_matches = np.insert(objects_matches, tmp_loop_count, np.array(figaro_objects_pattern.findall(om)), axis = 0 )
        # objects_matches[tmp_loop_count+1, -1]="$$$$$$$$$$$$$$"
#         print("\n------------------------------\n",om)
    tmp_loop_count  += 1
    
for i in range(len(objects_matches)):
    if(objects_matches[i,-1].find('OBJECT ', ) >=0 and objects_matches[i,-1].find('OBJECT ', ) <= 5):
        objects_matches[i,-1] = ""
# print(om_count)


for object_match in objects_matches:
    object_name = object_match[0]
    object_type = object_match[1]
    
    
#    print(' * Object name : {} | type : {} =========================================\n'.format(object_name, object_type))
    
    
    members_matches = figaro_members_pattern.findall(object_match[2])
    
    for member_match in members_matches:
    #     # member_match = members_matches[4]
        attribute = []
        if member_match[0] == 'INTERFACE':
            interface = figaro_member_declaration_pattern.findall(member_match[1])
            interface_name = interface[0][0]
            interface_membres = interface[0][1].split()
            # print('\t * Interface : {} '\
            #       '\n\t\t Membres : {} \n'.format(interface_name, interface_membres))   
        
        if member_match[0] == 'CONSTANT':
            constants = figaro_constant_declaration_pattern.findall(member_match[1])
            
            # they can be defined one after one another
            for constant in constants:
                
                constant_name = constant[0]
                constant_domain = constant[1]
                # in the case of many states are given: 
                if len(constant_domain.split(' ')) > 1:
                    constant_domain = re.findall(r"'(\w*?)'", constant_domain,flags=re.DOTALL|re.UNICODE)
                constant_value = constant[2]
                constant = list(constant)
                constant.append(object_name)
                constant = tuple(constant)
                all_constants.append(constant)
                # print(constant)
                
                    

                
        elif member_match[0] == 'ATTRIBUTE':
            match = list(member_match[1].split(";"))
            # print(match)
            attributes_found = []
            for i in range(len(match)):
                
                if len(match[i]) > 0:
                    attributes_found.append(match[i] + ';')
            
            
            
            for atbt in attributes_found:
                
                attribute = identifier_general(atbt, keywords_attribute)[0]
                
                if len(attribute)>0:
                    attribute_name = attribute[0]
                    # print('\t * Attribute : {}'.format(attribute_name))
                    
                    attribute_cpp_line = "\n"
                    for i in attribute[1:]:
                        
                        attribute_identifier = i.split()[0]
                        # print('\t\t * {} : {}'.format(attribute_identifier, i))
                        
                        # Translating to C++
                        if attribute_identifier =='LABEL':
                            i = i.replace (attribute_identifier,'')
                            i = i.replace('%OBJECT',object_name)
                            i = remove_specials(i)
                            attribute_cpp_line+= currentIndentation + "//"+i+"\n"
                        elif attribute_identifier =='DOMAIN':
                            i = i.replace (attribute_identifier,'')
                            if i.split()[0]=='BOOLEAN':
                                attribute_cpp_line+= currentIndentation + 'bool'
                                cpp_declaration_attributes += 'bool'
                                i = i.replace('BOOLEAN','')
                                i = change_boolean(i)
                                
                            elif i.split()[0]=='INTEGER':
                              attribute_cpp_line += currentIndentation + 'int'
                              cpp_declaration_attributes += 'int'
                              i = i.replace('INTEGER','')
                              i = change_boolean(i)
                              
                            elif i.split()[0]=='REAL':
                              attribute_cpp_line += currentIndentation + 'float'
                              cpp_declaration_attributes += 'float'
                              i = i.replace('REAL','')
                              i = change_boolean(i)
                              
                              
                            # save the other values
                            # for the enumerated variables  
                            else:
                              attribute_cpp_line += currentIndentation + i.replace("\n","").replace("\t","")
                              
                               # Handling the enum variables state
                              tmp = i.replace("\n","").replace("\t","").split("=")[0]
                              tmp = tmp.split("'")
                              for elem in tmp:
                                  elem = elem.replace(' ','')
                                  if len(elem)> 0 :
                                      if elem not in enum_list:
                                          enum_list.append(elem)
                              # ----
                              
                              i = " = " + i.replace("\n","").replace("\t","").split("=")[-1].replace("'","")
                              attribute_cpp_line = ""
                              
                             
                            i = remove_specials(i)
                            cpp_declaration_attributes += " "+attribute_name+"_OF_"+object_name+";\n"
                                               
                            attribute_cpp_line+=" "+attribute_name+"_OF_"+object_name+i+";\n"
                            
                        elif attribute_identifier == 'REINITIALISATION':
                            i = change_boolean(i)
                            i = i.split()
                            # attribute_cpp_line += currentIndentation + 'bool ' + attribute_name+'_OF_'+object_name +';\n'
                            attribute_cpp_line += currentIndentation + 'bool '+i[0]+'_OF_'+attribute_name+'_OF_'+object_name+' = '+i[1]+";\n"
                            cpp_declaration_attributes += 'bool '+i[0]+'_OF_'+attribute_name+'_OF_'+object_name+';\n'
                            cpp_all_reinitialisations+=(str(i[0])+'_OF_'+attribute_name+'_OF_'+object_name+';\n')
                    # print(attribute_cpp_line)
                    cpp_all_attributes+=attribute_cpp_line+'\n'
#                    cpp_all_attributes = cpp_all_attributes.replace("'","\"") 
                 
#                print(attribute_cpp_line)
  
                
        elif member_match[0] == 'INTERACTION':
            
            interactions = re.findall(r"\s*(.*?)\s*(?<=;)", member_match[1],flags=re.DOTALL|re.UNICODE)
            
            # print("\n=========================================\n" +member_match[1] + "\n--------------\n")
            # print(interactions)
            
            for ji in interactions:
                # print(identifier_general(i, keywords_interaction))
                # print('\n----------------------------------------------\n')
                interaction_cpp_line = ""
                if len(ji)>0:
                    interaction = identifier_general(ji, keywords_interaction)[0]
                    # print('#####' , interaction)
                    
                    
                    step = "default_step"
                    for i in interaction:  
                            
                        interaction_identifier =""
                        if len(i) > 0:
                            interaction_identifier = i.split()[0]
                            # print("+++",interaction_identifier)
                        
                        # translating to C++
                        if interaction_identifier =='STEP':
                            step = i.replace("STEP ",'')
                            step = step.replace(" ","")
                            if step not in step_names:
                                # print(step)
                                step_names.append(step)
                                cpp_interractions_separated_steps.append([])
                                
                        
                        if interaction_identifier == 'IF':
                            
                            
                                
                            if i.find("AT_LEAST") > -1:
#                                print(i)
                                i = remove_specials(i)
                                within_exp = ""
                                within_exp = re.findall("\s*WITHIN \((.*?)\)\s*", i)[0]
                                i = i.replace("AT_LEAST ","").replace(" WITHIN ", " <= ")
                                i = i.replace(within_exp, within_exp.replace(", ", " + "))
#                                print(i)
                            

                            i = change_boolean(i)
                            i = i.replace('IF ','if (')
                            i = i+' )'
                            # i = i.replace(' OF ','_OF_')
                            i = i.replace('NOT ', ' !')
                            i = i.replace('NOT\n', ' !')
                            i = i.replace(' =',' ==')
                            i = replaceRobust(i, 'OR', ' || ')
                            i = replaceRobust(i, 'AND', ' && ')
                            i = remove_specials(i)
                            
                            # handle the string equality 
                            while i.find("'") > 0:
#                                print(i)
                                i = i.replace("'","")
#                                print(i + "\n--------------\n")
                            
                            
                            interaction_cpp_line+= currentIndentation + i
                            # print("<<<",i)
                        if interaction_identifier == 'THEN':
                            # print(">>>\n",i)
                            # currentIndentation += '\t'
                            potential_else = ""
                            if i.find("ELSE") > -1:
#                                print(i)
                                potential_else += i.split("ELSE")[-1]                                
                                i = i.replace("ELSE","").replace(potential_else,"")
                                
                                potential_else = potential_else.replace(",\n",";\n")
                                potential_else = change_boolean(potential_else)
                                potential_else = remove_specials(potential_else)
                                potential_else = " else {" +potential_else +"; }\n"
                            
                            
                                
                            i = change_boolean(i)
                            i = i.replace(",\n",";\n")
                            i = remove_specials(i)
                            i = i.replace('+',' + ')
                            i = i.replace("'","") 
                                
                            if(i.find("MIN")> -1):
                                
                                i = i.replace("MIN", "fmin")
                                i = i.replace(";",",")

                                fmin_start_index = 0
                                pos_fmin = i.find("fmin(")
                                pos_coma = i.find(",", pos_fmin+len("fmin("))
                                pos_coma_2 = i.find(",", pos_coma+1)
                                pos_coma_3 = i.find(",", pos_coma_2+1)
                                pos_obracket = i.find("(", pos_coma)
                                pos_cbracket = i.find(")", pos_coma)
#                                print("<<< ", i)
                                while pos_coma_2 > -1 and pos_coma_3 > -1:
                                    pos_fmin = i.find("fmin(", fmin_start_index)                                    
                                    pos_coma = i.find(",", pos_fmin+len("fmin("))
                                    pos_coma_2 = i.find(",", pos_coma+1)
                                    pos_coma_3 = i.find(",", pos_coma_2+1)
                                    pos_obracket = i.find("(", pos_coma)
                                    pos_cbracket = i.find(")", pos_coma)
                                    
                                    
                                    if  (pos_coma_2 < pos_obracket or pos_coma_2 > pos_cbracket):                                    
                                        fmin_start_index = pos_fmin +4
                                    elif pos_coma_3 > -1:
                                        i = i.replace(i[pos_coma:pos_coma_2+1], i[pos_coma:pos_coma_2] +", fmin(" ) + ")"
                                        fmin_start_index = pos_fmin +4
                                
#                                print(">>> ", i)
                                        
                                        
                            if i.find("MAX") > -1:
                                
                                i = i.replace("MAX", "fmax")
                                i = i.replace(";",",")
                                
                                fmax_start_index = 0
                                pos_fmax = i.find("fmax(")
                                pos_coma = i.find(",", pos_fmax+len("fmax("))
                                pos_coma_2 = i.find(",", pos_coma+1)
                                pos_coma_3 = i.find(",", pos_coma_2+1)
                                pos_obracket = i.find("(", pos_coma)
                                pos_cbracket = i.find(")", pos_coma)
#                                print("<<< ", i)
                                while pos_coma_2 > -1 and pos_coma_3 > -1:
                                    pos_fmax = i.find("fmax(", fmax_start_index)                                    
                                    pos_coma = i.find(",", pos_fmax+len("fmax("))
                                    pos_coma_2 = i.find(",", pos_coma+1)
                                    pos_coma_3 = i.find(",", pos_coma_2+1)
                                    pos_obracket = i.find("(", pos_coma)
                                    pos_cbracket = i.find(")", pos_coma)
                                    
                                    
                                    if  (pos_coma_2 < pos_obracket or pos_coma_2 > pos_cbracket):                                    
                                        fmax_start_index = pos_fmax +4
                                    elif pos_coma_3 > -1:
                                        i = i.replace(i[pos_coma:pos_coma_2+1], i[pos_coma:pos_coma_2] +", fmax(" ) + ")"
                                        fmax_start_index = pos_fmax +4
                                        
#                                print(">>> ", i)
                                
                                
                                
                            if interaction_cpp_line.find('if ')>-1:
                                i = i.replace('THEN ','\n'+currentIndentation+'{\n'+currentIndentation+'\t')
                                
                                i = i.replace(';', ';\n'+currentIndentation+'\t')
                                # currentIndentation = currentIndentation.replace('\t','',1)
                                i +=";\n"+currentIndentation+"}"+ potential_else +"\n\n"
                            else:
                                i = i.replace('THEN ','\n\n\t')
                                i +="  ;\n\n"
                                
                            interaction_cpp_line += i 
                            # print("<<<\n",i)
                            
                cpp_interractions_separated_steps[step_names.index(step)].append(interaction_cpp_line)
                
                cpp_all_interactions+=interaction_cpp_line
            
        
            
                
        elif member_match[0] == 'OCCURRENCE':
            occurrences = np.array(figaro_occurrence_pattern.findall(member_match[1]))
            
            for occurrence in occurrences:
                may_occur_cpp = ""
                fire_occurrence_declaration = ""
                occurrence_comment = ""
                # print(occurrence)
                
                
                occurrence_name = remove_specials(occurrence[1].split("GROUP")[0])
                occurrence_if = occurrence[2]
                
                
                # translating to C++:
                occurrence_cpp_line = "\n"+currentIndentation+"// Occurrence "+occurrence_name+"_OF_"+object_name+"\n"
                occurrence_if = change_boolean(occurrence_if)
                # print(occurrence_if)
                # occurrence_if = occurrence_if.replace(' OF ','_OF_')
                occurrence_if = occurrence_if.replace(' AND ',' && ' )
                occurrence_if = occurrence_if.replace(' AND\n',' &&\n' )
                occurrence_if = occurrence_if.replace(' OR ',' || ' )
                occurrence_if = occurrence_if.replace('NOT ', ' !')
                occurrence_if = occurrence_if.replace('NOT\n', ' !')
                occurrence_if = occurrence_if.replace("		  ","")
                occurrence_if = occurrence_if.replace("\n\'"," \'")
                occurrence_if = occurrence_if.replace('=','==')
                occurrence_if = occurrence_if.replace('>==','>=')
                occurrence_if = occurrence_if.replace('<==','<=')
                occurrence_if = occurrence_if.replace("'","")
                    
                
                
                occurrence_if = currentIndentation + "if ("+occurrence_if +") \n"+currentIndentation+"{\n"
                # occurrence_cpp_line += occurrence_if
                
                occurrence_may_occur = occurrence[3] + ";"# v0.2
                # print("\n", occurrence[3])
                # print('===> ', occurrence_may_occur)
                fire_occurrence_declaration =  "FIRE_"+occurrence_name+"_OF_"+object_name 
                
                # print( "---------------------------------------\n", occurrence_may_occur)
                # v0.5
                occurrence_may_occur = occurrence_may_occur.replace('\n','').replace('\t','')
                ins_cpp_line = ""
                # if(occurrence_may_occur.find("OR_ELSE")>=0):
                if(occurrence_may_occur.find(" INS ")>=0):
                    # print("=== INS ===")
                    occurrences_ins = re.findall("\s*(.*?)\s*;", occurrence_may_occur)[0]
                    
                    ins_total_count += 1
                    
                    # v0.5
                    for oi in occurrences_ins.replace("%OBJECT", object_name).split("OR_ELSE"):
                        ins_parsed = list(figaro_INS_pattern.findall(oi)[0])
                        ins_parsed[-1] = ins_parsed[-1] + ";"
                        ins_parsed = np.array(ins_parsed)
                        ins_parsed[-1] = str(ins_parsed[-1] + ";")
                        # print(len(figaro_INS_pattern.findall(oi)[0])," : ",figaro_INS_pattern.findall(oi)[0])
                        fire_ins_declaration = ""
                        may_occur_ins_cpp = ""
                        ins_comment = ""
                        
                        ins_comment += "cout << \"" + str(occurrence_count) + " : "
                        ins_comment += " INS_SUB_COUNT ("+ str(occurrence_sub_count) +") |"
                        for ins_elem in ins_parsed[:-1]:
                            ins_comment += ins_elem.replace("\"","\\\"") + " | "
                        ins_comment += ins_parsed[-1]+"\" <<endl;"
                        ins_comment = ins_comment.replace("%OBJECT", object_name)
                        
                        
                                            
                        fire_ins_declaration = fire_occurrence_declaration+"_INS_"+str(occurrence_count)
                        fire_occurrence_list.append([fire_ins_declaration, ins_comment])
                        cpp_all_fire_occurrences += currentIndentation + 'bool ' + fire_ins_declaration + ' = false;\n'
                        
                        ins_cpp_line += "\n"+occurrence_if.replace('\t','') 
                        
                        ins_cpp_line += "\n\t" + ins_comment
                        
                        ins_cpp_line += "\n\tif (" + fire_ins_declaration+") \n\t{\n\t\t"
                        
                        may_occur_ins_cpp = re.findall("INDUCING\s*(.*?)\s*;", ins_parsed[-1])[0]
                        may_occur_ins_cpp = may_occur_ins_cpp.replace(',',';\n		')
                        may_occur_ins_cpp = change_boolean(may_occur_ins_cpp)
                        may_occur_ins_cpp += ";"
                        # may_occur_ins_cpp += "\n"+currentIndentation +"\t" + instantaneous_variables[-1]  + " = false;"
                        may_occur_ins_cpp += "\n"+currentIndentation +"\t" + fire_ins_declaration + " = false;"
                        
                        
                        
                        ins_cpp_line += may_occur_ins_cpp +'\n\t}\n'#'\n}\n'
                        
                        ins_cpp_line += "\n}" 
                        # print(ins_cpp_line,"\n=============>\n",occurrence_sub_count)
                        # print(ins_cpp_line)
                        
                        occurrence_count+=1                    
                    
                    occurrence_sub_count += 1
                    # print(ins_cpp_line)
                        
                    
                    # print("=== /INS ===")
                    
                    
                else:
                    
                    occurrence_cpp_line += occurrence_if
                    """ """
                    occurrence_comment = "cout << \""+str(occurrence_count)+" : "+occurrence_name+"_OF_"+object_name+" :"\
                        " "+str(occurrence_may_occur.replace('\n\t\t',' | ').replace('"','\\"'))+"\" <<endl; \n"
                        
                    occurrence_comment = occurrence_comment.replace("%OBJECT", object_name)

                    occurrence_cpp_line += currentIndentation+"\t"+occurrence_comment
                                            
                    # fire_occurrence_declaration =  "FIRE_"+occurrence_name+"_OF_"+object_name 
                    
                    fire_occurrence_list.append([fire_occurrence_declaration, occurrence_comment])
                    
                    cpp_all_fire_occurrences += currentIndentation + 'bool '+fire_occurrence_declaration + ' = false;\n'
                    
                    occurrence_cpp_line += currentIndentation+"\tif ("+fire_occurrence_declaration+")\n"+currentIndentation+"\t{\n"
                    
                    """ """
                    may_occur_cpp = re.findall("INDUCING\s*(.*?)\s*;", occurrence_may_occur)[0]
                    may_occur_cpp = may_occur_cpp.replace(',',';\n			')
                    # print('===> ===> ', may_occur_cpp)
                    
                    
                        
                    may_occur_cpp = change_boolean(may_occur_cpp).replace("'", "") 
                    
                    # may_occur_cpp = may_occur_cpp.replace(may_occur_cpp.split()[0], "boolState[" + may_occur_cpp.split()[0] + "]") #v0.3
                    may_occur_cpp += ';'
                    # print(may_occur_cpp)
                    may_occur_cpp += "\n" + currentIndentation +  "\t\tFIRE_"+occurrence_name+"_OF_"+object_name+" = false;"
                    occurrence_cpp_line += currentIndentation+'\t\t'+may_occur_cpp+'\n\t'+currentIndentation+'}\n'+currentIndentation+'}\n'
                    # occurrence_cpp_line += currentIndentation+'}\n'
                    occurrence_count+=1                    
                    
                    
                # occurrence_cpp_line += currentIndentation+'\t\t'+may_occur_cpp+'\n\t'+currentIndentation+'}\n'+currentIndentation+'}\n'
                occurrence_cpp_line += ins_cpp_line.replace("\n","\n\t").replace("'","")
                
                # print(occurrence_cpp_line)
                cpp_all_occurrences += occurrence_cpp_line
                
                
                
    
        
# translating to C++ of Constants
for i in list(set(all_constants)): # don't take into account duplicates
    constant_cpp_line = ""
    # print('>>>\n',i)
    interaction_domain = i[1]
    if len(interaction_domain.split())>1:
        interaction_domain = 'std::string'
        
    
    interaction_domain = interaction_domain.replace('BOOLEAN','bool')
    interaction_domain = interaction_domain.replace('REAL','double')
    interaction_domain = interaction_domain.replace('INTEGER','int')
    constant_cpp_line = currentIndentation + interaction_domain+' const '+i[0]+'_OF_'+i[3]+' = '+i[2].replace('\'','"')+';'
    constant_cpp_line = change_boolean(constant_cpp_line)
    
    cpp_all_constants+=constant_cpp_line+"\n"
    
    # print('<<<<\n',constant_cpp_line)
    

input_file.close()

# Modification for variable declaration : ===========================
cpp_declaration_fire = cpp_all_fire_occurrences
cpp_declaration_fire = cpp_declaration_fire.replace(' = ','')
cpp_declaration_fire = cpp_declaration_fire.replace('false','')
cpp_declaration_fire = cpp_declaration_fire.replace('true','')

cpp_declaration_pre_attributes = cpp_declaration_attributes
cpp_declaration_pre_attributes = cpp_declaration_pre_attributes.replace('ATTRIBUTS ', 'PRE_ATTRIBUTS')
cpp_declaration_pre_attributes = cpp_declaration_pre_attributes.replace('int ', 'int pre_')
cpp_declaration_pre_attributes = cpp_declaration_pre_attributes.replace('bool ', 'bool pre_')

#==============================================================================

# Writing of the file: cpp/FigaroModel.cpp ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cppSourceFile = open(cpp_file_path,"w")
cppSourceFile.write("""

#include <iostream>

#include "FigaroModel.h"


using namespace std;





namespace storm{
    namespace figaro{
        /* ---------- DECLARATION OF OCCURRENCE RULES FIRING FLAGS ------------ */
//        storm::figaro::FigaroProgram1::FigaroProgram1()
//        {
//        for(int i=0; i < numBoolState; i++)
//            boolState[i]=0;
//        
//        }
""")



### Variable declaration
indent = '\n'
currentEdition = ""

## pre_attributes  déclaration
cpp_pre_attributes_list = re.findall(r'\n\s*(.*?)\s*;',cpp_declaration_pre_attributes)
cpp_all_pre_attributes =""
cpp_all_pre_attributes += indent + "/* ---------- DECLARATION OF ATTRIBUTES USED TO MEMORIZE STATE ------------ */"



list_boolState = []
list_floatState = []
list_intState = []
list_enumState = []

def replaceByElemState(expression, stateName, listOfElem):
    """
    Replace the attribute from listOfElem in 'expression' by 'stateName['+attribute+']'.
    
    Parameters
    ----------
    expression : TYPE
        DESCRIPTION.

    Returns
    -------
    expression : TYPE
        DESCRIPTION.

    """
    for elem in listOfElem:
        if expression.find(elem) >= 0:
            expression = expression.replace('('+elem+' ', "(" + stateName + "[" + elem + "] ")
            expression = expression.replace('('+elem+'=', "(" + stateName + "[" + elem + "] =")
            expression = expression.replace(' '+elem+'=', " " + stateName + "[" + elem + "] =")
            expression = expression.replace(' '+elem+')', " " + stateName + "[" + elem + "])")
            expression = expression.replace('('+elem+')', "(" + stateName + "[" + elem + "])")               
            expression = expression.replace(' '+elem+' ', " " + stateName + "[" + elem + "] ")
            expression = expression.replace("			" +elem+' ', "			" + stateName + "[" + elem + "] ")
            expression = expression.replace('\t'+elem+' ', "\t" + stateName + "[" + elem + "] ")
            expression = expression.replace(' '+elem+'\t', " " + stateName + "[" + elem + "] ")
            expression = expression.replace('\t'+elem+')', "\t" + stateName + "[" + elem + "])")
            expression = expression.replace('\n'+elem+')', "  " + stateName + "[" + elem + "])")  
            expression = expression.replace('!'+elem+' ', "!" + stateName + "[" + elem + "] ")
            expression = expression.replace('!'+elem+')', "!" + stateName + "[" + elem + "])")
            
            expression = expression.replace('&&'+elem+' ', "&& " + stateName + "[" + elem + "] ")       
            expression = expression.replace('&&'+elem+')', "&& " + stateName + "[" + elem + "])")
            expression = expression.replace('('+elem+'&&', "(" + stateName + "[" + elem + "] &&")       
            expression = expression.replace(' '+elem+'&&', " " + stateName + "[" + elem + "] &&") 
            expression = expression.replace('!'+elem+'&&', "!" + stateName + "[" + elem + "] &&")       
            expression = expression.replace('\n'+elem+'&&', " " + stateName + "[" + elem + "] &&")  
            expression = expression.replace(' '+elem+'&&', " " + stateName + "[" + elem + "] &&")
            
            expression = expression.replace('('+elem+'\n', "(" + stateName + "[" + elem + "]\n")
            expression = expression.replace('('+elem+'        ', "(" + stateName + "[" + elem + "] ")
            expression = expression.replace('('+elem+'\t', "(" + stateName + "[" + elem + "] ")
            expression = expression.replace('!'+elem+'\t', "!" + stateName + "[" + elem + "] ")
            
            expression = expression.replace('('+elem+ ',', "(" + stateName + "[" + elem + "],")
            expression = expression.replace(' '+elem+ ',', " " + stateName + "[" + elem + "],")
            expression = expression.replace(' '+elem+ ';', " " + stateName + "[" + elem + "];")
            
    return expression

def ALLreplaceByElemState(expression):
    expression = replaceByElemState(expression, "boolState", list_boolState) 
    expression = replaceByElemState(expression, "floatState", list_floatState)
    expression = replaceByElemState(expression, "intState", list_intState)
    expression = replaceByElemState(expression, "enumState", list_enumState)
    expression = replaceByElemState(expression, "boolFailureState", list_failureIndex)
    return expression


def parseFigaroAnalysisExpRAW(exp):
    
    exp = exp.replace("(","_OF_").replace(")","")
    
    # creating the variable name corresponding to exp (not currently used)
    var_name = exp
    var_name = var_name.replace(" =", "_iseq_")
    var_name = var_name.replace(">==","_issup_or_eq_")
    var_name = var_name.replace("<==","_isinf_or_eq_")
    
    var_name = var_name.replace(" ","_")
    var_name = var_name.replace("__","_")
    var_name = var_name.replace("NOT","not")
    
    
    # Handling of exp    
    exp = exp.replace('NOT ', ' !')
    exp = exp.replace('NOT\n', ' !')
    exp = exp.replace(' =',' ==')
    exp = replaceRobust(exp, 'OR', ' || ')
    exp = replaceRobust(exp, 'AND', ' && ')
    
    exp = ALLreplaceByElemState(" "+ exp +" ")
    return exp


cpp_reinitState = ""
sspace = "                    "
# For booleans :    
cpp_boolState = ""
cpp_map_boolState = "//            std::map<std::string, size_t> mFigaroboolelementindex =\n"+sspace+"{  "

index_boolState = 0

# For floats : 
cpp_floatState = ""
cpp_map_floatState = "//            std::map<std::string, size_t> mFigarofloatelementindex =\n"+sspace+" {  "
index_floatState = 0

# For integers : 
cpp_intState = ""
cpp_map_intState = "//            std::map<std::string, size_t> mFigarointelementindex =\n"+sspace+" {  "
index_intState = 0

# For enums : 
cpp_enumState = ""
cpp_map_enumState = "//            std::map<std::string, size_t> mFigaroenumelementindex =\n"+sspace+" {  "
cpp_enum_var_names = "//            std::set<std::string> enum_variables_names =\n"+sspace+" {  "
cpp_float_var_names = "//            std::set<std::string> float_variables_names =\n"+sspace+" {  "
index_enumState = 0

for att in re.findall(r'\n\s*(.*?)\s*;',cpp_declaration_attributes): 
    if att.find("REINITIALISATION_OF_") < 0:
                
        if att.find("bool ") >= 0 :
            cpp_boolState += indent + att.replace("bool ","int ") + " = " + str(index_boolState) +" ;"
            list_boolState.append(att.replace("bool ","") )
            cpp_map_boolState += "\n            \t{\"" + att.replace("bool ","") +"\" , " +str(index_boolState) + "},"
            
#            if(att.replace("bool ","").startswith("S_OF_") or att.replace("bool ","").startswith("fail_OF_")):

                
            index_boolState += 1
            
        elif att.find("float ") >= 0:
#            print("float : ", att)
            cpp_floatState += indent + att.replace("float ","int ") + " = " + str(index_floatState) +" ;"
            list_floatState.append(att.replace("float ","") )
            cpp_map_floatState += "\n            \t{\"" + att.replace("float ","") +"\" , " +str(index_floatState) + "},"
            cpp_float_var_names+=  "\n            \t\"" + att.replace("float ","") +"\" ,"
            index_floatState += 1
        
        elif att.find("int ") >= 0:
#            print("int : ", att)
            cpp_intState += indent + att + " = " + str(index_intState) +" ;"
            list_intState.append(att.replace("int ","") )
            cpp_map_intState += "\n            \t{\"" + att.replace("int ","") +"\" , " +str(index_intState) + "},"
            index_intState += 1
            
        # enum variables : 
        else:
#            print(" raw enum : ", att)
            att = att.split(" ")[-1]
            cpp_enumState += indent + "int " + att + " = " + str(index_enumState) +" ;"
            list_enumState.append(att)
            cpp_map_enumState +=  "\n            \t{\"" + att +"\" , " +str(index_enumState) + "},"
            cpp_enum_var_names+=  "\n            \t\"" + att +"\" ,"
            index_enumState += 1
            
            

    else:
        cpp_reinitState += indent + att + " ;"
        

       
        
# Handling of the specified condition from the analysis file
cpp_failureIndex = ""        
cpp_map_failureIndex = "//            std::map<std::string, size_t> mFigaroelementfailureindex =\n"+sspace+"{  "
cpp_failureIndex_var_names = "//            std::map<std::string, size_t> failure_variable_names =\n"+sspace+"{  "
index_failureIndex = 0
list_failureIndex = []
tree = ET.parse(input_analysis_path)
root = tree.getroot()
failure_elements = []
for child in root.find("UNRELIABILITY"):
    failure_name = "exp"+str(index_failureIndex)
    cpp_map_failureIndex += "{ \""+failure_name+"\","+str(index_failureIndex)+"},"
    list_failureIndex.append(failure_name)
    failure_elements.append([failure_name, parseFigaroAnalysisExpRAW(child.get("CONDITION"))] )
    
    cpp_failureIndex+="\nint exp"+str(index_failureIndex)+" = " + str(index_failureIndex)+" ;"
    cpp_failureIndex_var_names += "\"" + failure_name + "\"" + ","
    
    
    index_failureIndex += 1

# TODO: handle the top event more precisely:
cpp_top_event = list_failureIndex[0]


        
        
numBoolState = index_boolState
numFloatState = index_floatState
numIntState = index_intState
numEnumState = index_enumState
numFailureIndex = index_failureIndex

cpp_map_failureIndex = cpp_map_failureIndex[0:-1] + "},\n"  
cpp_failureIndex_var_names = cpp_failureIndex_var_names[0:-1] + "},\n"  
cpp_map_boolState = cpp_map_boolState[0:-1] + "},\n"
cpp_map_floatState = cpp_map_floatState[0:-1] + "},\n"
cpp_map_intState = cpp_map_intState[0:-1] + "},\n"
cpp_map_enumState = cpp_map_enumState[0:-1] + "},\n"
cpp_enum_var_names = cpp_enum_var_names[0:-1] + "},\n"
cpp_float_var_names = cpp_float_var_names[0:-1] + "},\n"







"""
===============================================================================================
"""

### Initialization function declaration
indent = '\n\t'
currentEdition = "\nvoid storm::figaro::FigaroProgram1::init()\n{"
# {
currentEdition += indent + "cout <<\">>>>>>>>>>>>>>>>>>>> Initialization of variables <<<<<<<<<<<<<<<<<<<<<<<\" << endl;\n"
cpp_all_attributes_list = re.findall(r"\n\s*(.*?)\s*=\s*(.*?)\s*;" , cpp_all_attributes) # ne prend pas les valeurs non assigné lors de l'initialisation ainsi
for att in cpp_all_attributes_list:
#     if att[0].find("REINITIALISATION_OF_") < 0:
#         currentEdition += indent +"boolState[" + att[0] + "] = " + att[1] + ";"
     currentEdition += indent + att[0] + " = " + att[1] + ";"

#     else:
#        currentEdition += indent + att[0] + " = " + att[1] + ";"

currentEdition = ALLreplaceByElemState(currentEdition)
    
currentEdition += "\n"+cpp_all_fire_occurrences
currentEdition+=("\n}") 
# }
currentEdition = currentEdition.replace('int ','')
currentEdition = currentEdition.replace('bool ','')
currentEdition = currentEdition.replace('double ','')
currentEdition = currentEdition.replace('float ', '')
currentEdition = currentEdition.replace("\n\n\n","\n")
cppSourceFile.write(currentEdition)


### Saving current state function declaration
cpp_pre_equal_attribute = cpp_declaration_attributes
cpp_pre_equal_attribute = cpp_pre_equal_attribute.replace('int ', '')
cpp_pre_equal_attribute = cpp_pre_equal_attribute.replace('bool ', '')
cpp_pre_equal_attribute = cpp_pre_equal_attribute.replace('float ', '')
attribute_list = re.findall(r'\n\s*(.*?)\s*;',cpp_pre_equal_attribute)
cpp_pre_equal_attribute = ""
indent = '\n\t'
for att in attribute_list:
    if att.startswith("REINITIALISATION_OF_")==False:
        cpp_pre_equal_attribute += "\n\tpre_"+att+" = "+att+";"


currentEdition = "\n\nvoid storm::figaro::FigaroProgram1::saveCurrentState()\n{\n\t// cout <<\">>>>>>>>>>>>>>>>>>>> Saving current state  <<<<<<<<<<<<<<<<<<<<<<<\" << endl;"
currentEdition += "\n\tbackupBoolState = boolState ;"
currentEdition += "\n\tbackupFloatState = floatState ;"
currentEdition += "\n\tbackupIntState = intState ;"
currentEdition += "\n\tbackupEnumState = enumState ;"



currentEdition+=("\n}") 
# }
cppSourceFile.write(currentEdition)

##  Comparing state function declaration
cpp_state_comparaison = cpp_pre_equal_attribute
cpp_state_comparaison = cpp_state_comparaison.replace("pre_", "differences_count += (pre_")
cpp_state_comparaison = cpp_state_comparaison.replace(' = ',' != ')
cpp_state_comparaison = cpp_state_comparaison.replace(';',');')

currentEdition = "\n\nint storm::figaro::FigaroProgram1::compareStates()\n{" 
indent = '\n\t'
# {
currentEdition += indent + "// cout <<\">>>>>>>>>>>>>>>>>>>> Comparing state with previous one (return number of differences) <<<<<<<<<<<<<<<<<<<<<<<\" << endl;\n"

# v0.3
currentEdition += indent + "return (backupBoolState != boolState) + (backupFloatState != floatState) + (backupIntState != intState) + (backupEnumState != enumState); "
currentEdition+=("\n}") 

# }
cppSourceFile.write(currentEdition)


### Print of the current state function declaration
currentEdition = "\n\nvoid storm::figaro::FigaroProgram1::printState()\n{" 
currentEdition += indent + "cout <<\"\\n==================== Print of the current state :  ====================\" << endl;\n"
for att in attribute_list:
    if att.startswith("REINITIALISATION_OF_") == False:
        currentEdition+="\n\tcout << \"Attribute :  {} | Value : \" << {} << endl;".format(att,att)
currentEdition = ALLreplaceByElemState(currentEdition)
currentEdition+=("\n}") 
cppSourceFile.write(currentEdition)

### checking if figaro model has inst transitions
currentEdition = "\n\nbool storm::figaro::FigaroProgram1::figaromodelhasinstransitions()\n{"
# currentEdition += indent + "cout <<\"\\n==================== Checking if we have instantaenous transitions :  ====================\" << endl;\n"
if ins_total_count > 0:
    currentEdition += "\n\treturn true;".format(att, att)
else:
    currentEdition += "\n\treturn false;".format(att, att)
currentEdition += ("\n}")
cppSourceFile.write(currentEdition)

### Reinitialization function declaration
currentEdition = "\n\nvoid storm::figaro::FigaroProgram1::doReinitialisations()\n{" 
indent = '\n\t'
all_reinitialisations = re.findall(r"REINITIALISATION_OF_\s*(.*?)\s*;",cpp_all_reinitialisations)
for r in all_reinitialisations:
#    currentEdition += indent + "boolState["+ r+ "]" + " = " + "REINITIALISATION_OF_" + r + ";" 
    currentEdition += indent +  r + " = " + "REINITIALISATION_OF_" + r + ";"    
currentEdition = ALLreplaceByElemState(currentEdition)
currentEdition+=("\n}") 
cppSourceFile.write(currentEdition)


### HANDLING OF OCCURRENCES
##  Fire of occurrence function declaration
fire_occurrence_list = np.array(fire_occurrence_list)
currentEdition = "\n\nvoid storm::figaro::FigaroProgram1::fireOccurrence(int numFire)\n{" 
# {
indent = '\n\t'
currentEdition += indent + "cout <<\">>>>>>>>>>>>>>>>>>>> Fire of occurrence #\" << numFire << \" <<<<<<<<<<<<<<<<<<<<<<<\" << endl;\n"
for numFo in range(len(fire_occurrence_list)):
    indent = '\n\t'
    currentEdition += indent +"if (numFire == {})".format(numFo)
    currentEdition += indent +"{"
    indent += "\t"
    currentEdition += indent + str(fire_occurrence_list[numFo, 0]) + " = true;"
    # currentEdition += indent + "break;"
    indent = indent.replace('\t\t','\t', 1)
   
    currentEdition += indent + "}\n"


occurrence_all_comments = re.findall(r"(?=cout)\s*(.*?)\s*(?<=;)",cpp_all_occurrences, flags=re.DOTALL|re.UNICODE)
cpp_all_occurrences_no_comment = cpp_all_occurrences
for oac in occurrence_all_comments:
    cpp_all_occurrences_no_comment = cpp_all_occurrences_no_comment.replace(oac+"\" <<endl;",'')
currentEdition += cpp_all_occurrences_no_comment#replace('cout','//cout')

currentEdition+=("\n}\n\n") 
# print(currentEdition)
# }

currentEdition = ALLreplaceByElemState(currentEdition)
cppSourceFile.write(currentEdition)

## showFireableOccurrences function declaration
raw_occurrences_conditions_list = re.findall(r"if\s\(\s*(.*?)\s*\)\s*{", cpp_all_occurrences.replace('\n',''))
occurrences_comment_list = re.findall(r"(?=cout\s<<)\s*(.*?)\s*(?=;)", cpp_all_occurrences)
occurrences_conditions_list = []
cpp_ins_fireable = ""
for ocl in raw_occurrences_conditions_list:
    if ocl.startswith('FIRE')==False:
        occurrences_conditions_list.append(ocl)
        
        
currentEdition = ""    
currentEdition_top = "std::vector<std::tuple<int, double, std::string, int>> storm::figaro::FigaroProgram1::showFireableOccurrences()\n{"
# {
indent = '\n\t'
currentEdition_top += indent + "std::vector<std::tuple<int, double, std::string, int>> list = {};"
currentEdition_top += indent + "cout <<\"\\n==================== List of fireable occurrences :  ====================\" << endl;\n"
assert len(occurrences_conditions_list)==len(occurrences_comment_list)
for i in range(len(occurrences_conditions_list)):
    tmp_cpp_line = ""
    tmp_cpp_line += indent + "if ("+occurrences_conditions_list[i] + ")"
    tmp_cpp_line += indent + "{"
    indent += '\t'
    tmp_cpp_line += indent + occurrences_comment_list[i] + "\" << endl;"
    tmp_rate = re.findall("\s*DIST (EXP|INS) \((.*?)\)\s*",occurrences_comment_list[i] ) # v0.5
    # print(tmp_rate)
    tmp_cpp_line += indent +"list.push_back(make_tuple(" + str(i) + ", " + tmp_rate[0][1] + ", \""+ tmp_rate[0][0] 
    
    # Handling of ins occurence group numbers
    if (occurrences_comment_list[i].find("EXP ")>=0):
        tmp_cpp_line += "\", 0" + "));"
    else:
        tmp_cpp_line += "\", " + re.findall("\s*INS_SUB_COUNT \((.*?)\)\s*",occurrences_comment_list[i])[0] + "));"
        
    indent = indent.replace('\t\t','\t', 1)
    tmp_cpp_line += indent + "}"
    
    if (occurrences_comment_list[i].find("EXP ")>=0):
        currentEdition += tmp_cpp_line
    else:
        cpp_ins_fireable += tmp_cpp_line

cpp_ins_fireable += """  
    if (list.size() > 0)
    {
		ins_transition_found = true;
		return list;
	}
	else
	{
		ins_transition_found = false;
	}
 """
currentEdition = currentEdition_top + cpp_ins_fireable + currentEdition    
currentEdition = ALLreplaceByElemState(currentEdition)
currentEdition += indent + "return list;"
currentEdition+=("\n}\n\n") 
# }
cppSourceFile.write(currentEdition)
# print(currentEdition)



### MANAGEMENT OF INTERRACTIONS

## setting the interaction's groups in the order specified by the figaro file
ordered_interaction_step_name = []
for ds in declared_steps:
    if ds in step_names:
        ordered_interaction_step_name.append(ds)
        
## writing each interaction groups in different functions
assert len(step_names) == len(cpp_interractions_separated_steps)
interaction_function_names = []
for s in ordered_interaction_step_name:
    if len(cpp_interractions_separated_steps[step_names.index(s)])>0:
        currentEdition = ""
        indent = '\n\t'
        interaction_function = "\nvoid storm::figaro::FigaroProgram1::runOnceInteractionStep_"+s+"()"
        interaction_function_names.append(interaction_function)
        currentEdition += interaction_function + "\n{\n"
        for inter in cpp_interractions_separated_steps[step_names.index(s)]:
            currentEdition += inter
        
        currentEdition = ALLreplaceByElemState(currentEdition)
        currentEdition += "}\n\n"
        # print(currentEdition + "\n ==========================================\n")
        cppSourceFile.write(currentEdition)
        
## function to run all the interactions in the right order
currentEdition = """void storm::figaro::FigaroProgram1::runInteractions() {
    int counter = 0;
    int comparator = 1;
    doReinitialisations();
    int max_interactions_loop = """ + root.find("PARAMETERS").get("MAX_RULES_RUNS")+";"

        
for ifn in interaction_function_names:
    
    currentEdition+= """ 
 	counter = 0;
 	comparator = 1;
	do
 	{
		//cout << counter << endl;
		saveCurrentState();"""
        
    currentEdition+= ifn.replace("void storm::figaro::FigaroProgram1::","\t\t") +";"
    
    currentEdition+="""\n\n		comparator = compareStates();
		counter++;

 	} while (comparator > 0 && counter < max_interactions_loop);
 	if (comparator <= 0)
 	{
		cout << "==> Stabilisation of interactions at loop #" << counter << " for """+ifn.replace("\nvoid storm::figaro::FigaroProgram1::", "").replace("nOnceI","nI") + """ ." << endl;
 	}
 	else {
		cout << "==> Maximum of interactions loop  reached : #" << counter <<" for """+ifn.replace("\nvoid storm::figaro::FigaroProgram1::", "")+ """." << endl;
 	}
     """
     
currentEdition += """
    // ------------------- Handling of FailureState element --------------------------------
"""
for fe in failure_elements:
    currentEdition += "\n\t"+fe[0] + " = (" + fe[1] + ");"

currentEdition = ALLreplaceByElemState(currentEdition)

currentEdition+="""
 	cout << endl;
}"""

# print(currentEdition)
cppSourceFile.write(currentEdition)

cppSourceFile.write("""
        void storm::figaro::FigaroProgram1::printstatetuple(){
            std::cout<<"\\n State information: (";
            for (int i=0; i<boolFailureState.size(); i++)
                {
                std::cout<<boolFailureState.at(i);
                }
            std::cout<<")";
            
        }
        int_fast64_t FigaroProgram1::stateSize() const{
            return numBoolState;
        }
        
        void storm::figaro::FigaroProgram1::fireinsttransitiongroup(std::string user_input_ins)
        {
         std::vector<int> list_user_inputs = {};
        int user_input = -2;
        stringstream ss(user_input_ins);
        for (int i; ss >> i;) {
            list_user_inputs.push_back(i);
            if (ss.peek() == ',')
                ss.ignore();
        }
        
        for (size_t i = 0; i < list_user_inputs.size(); i++)
            {
            cout << list_user_inputs[i] << endl;
            user_input = list_user_inputs[i];
            if (user_input == -1) {
                break;
            }
            fireOccurrence(user_input);
            }
        }
        
    }
}
""")

cppSourceFile.close() # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

### Writing of the header file: cpp/FigaroModel.h ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
cppHeaderFile = open(header_file_path,"w")
cppHeaderFile.write("""
#include "FigaroModelTemplate.h"                    
#pragma once
#include <array>
#include <map>
#include <vector>
#include <sstream> 
#include<math.h>
#include <set>

namespace storm{
    namespace figaro{
        class FigaroProgram1: public storm::figaro::FigaroProgram{
        public:
            FigaroProgram1(): FigaroProgram(
            
            
"""
)

currentIndentation = "            "

cppHeaderFile.write(cpp_map_boolState+"\n")
cppHeaderFile.write(cpp_map_failureIndex+"\n")
cppHeaderFile.write(cpp_map_floatState+"\n")
cppHeaderFile.write(cpp_map_intState+"\n")
cppHeaderFile.write(cpp_map_enumState+"\n")
cppHeaderFile.write(cpp_failureIndex_var_names+"\n")
cppHeaderFile.write(cpp_enum_var_names+"\n")
cppHeaderFile.write(cpp_float_var_names+"\n")






cppHeaderFile.write("\n//" + currentIndentation + "std::string const topevent=\n"+sspace+"\"" + cpp_top_event + "\",")


cppHeaderFile.write("\n//" + currentIndentation + "static int const numBoolState = \n"+sspace+str(numBoolState)+ " ,")
cppHeaderFile.write("\n//" + currentIndentation + " numBoolFailureState = \n"+sspace+str(numFailureIndex)+ " ,")
cppHeaderFile.write("\n//" + currentIndentation + "static int const numFloatState = \n"+sspace+str(numFloatState)+ " ,")
cppHeaderFile.write("\n//" + currentIndentation + "static int const numIntState = \n"+sspace+str(numIntState)+ " ,")
cppHeaderFile.write("\n//" + currentIndentation + "static int const numEnumState = \n"+sspace+str(numEnumState)+ " ,")
cppHeaderFile.write("\n//" + currentIndentation + "bool ins_transition_found = \n"+sspace+" false){} \n")


# enum variables declaration
currentEdition = "\n" + currentIndentation + "/* ---------- CODING ENUMERATED VARIABLES STATES ------------ */\n"
currentEdition += currentIndentation + "enum enum_status {  "
enum_number = 0
for elem in enum_list:
    currentEdition+= elem + " = " + str(enum_number) +", "
    enum_number += 1 
currentEdition = currentEdition[:-2]
cppHeaderFile.write(currentEdition + "};\n")


cppHeaderFile.write("\n//" + currentIndentation + "std::array<bool, numBoolState> boolState;")
cppHeaderFile.write("\n//" + currentIndentation + "std::array<bool, numBoolState> backupBoolState;")

cppHeaderFile.write("\n//" + currentIndentation + "std::array<float, numFloatState> floatState;")
cppHeaderFile.write("\n//" + currentIndentation + "std::array<float, numFloatState> backupFloatState;")

cppHeaderFile.write("\n//" + currentIndentation + "std::array<int, numIntState> intState;")
cppHeaderFile.write("\n//" + currentIndentation + "std::array<int, numIntState> backupIntState;")

cppHeaderFile.write("\n//" + currentIndentation + "std::array<int, numEnumState> enumState;")
cppHeaderFile.write("\n//" + currentIndentation + "std::array<int, numEnumState> backupEnumState;")






## attributes declaration
currentEdition = ""


currentEdition += cpp_reinitState 
currentEdition += "\n"
cppHeaderFile.write(currentEdition.replace('\n', '\n'+currentIndentation))

# Constant declaration: 
cppHeaderFile.write(cpp_all_constants.replace('\n','\n\t\t'))

# fire_occurrence declaration
# cpp_declaration_fire = cpp_declaration_fire.replace('\n\tbool ', '\nextern bool ')
cppHeaderFile.write(cpp_declaration_fire.replace('\n\t', '\n'+currentIndentation))

cppHeaderFile.write(cpp_boolState.replace('\n', '\n'+currentIndentation)+"\n")

cppHeaderFile.write(cpp_floatState.replace('\n', '\n'+currentIndentation)+"\n")

cppHeaderFile.write(cpp_intState.replace('\n', '\n'+currentIndentation)+"\n")

cppHeaderFile.write(cpp_enumState.replace('\n', '\n'+currentIndentation)+"\n")

cppHeaderFile.write(cpp_failureIndex.replace('\n', '\n'+currentIndentation)+"\n")

### Function header declaration :
cppHeaderFile.write("\n\n" + currentIndentation + "/* ---------- DECLARATION OF FUNCTIONS ------------ */")

cppHeaderFile.write("\n" + currentIndentation + "void init();")

cppHeaderFile.write("\n" + currentIndentation + "void saveCurrentState();")

cppHeaderFile.write("\n" + currentIndentation + "void printState();")

cppHeaderFile.write("\n" + currentIndentation + "void fireOccurrence(int numFire);" )

cppHeaderFile.write("\n" + currentIndentation + "std::vector<std::tuple<int, double, std::string, int>> showFireableOccurrences();")


# interaction test function
for ifn in interaction_function_names:
        cppHeaderFile.write(ifn.replace('\n', '\n'+currentIndentation).replace("storm::figaro::FigaroProgram1::", "")   + ";")

cppHeaderFile.write("\n" + currentIndentation + "int compareStates();")

cppHeaderFile.write("\n" + currentIndentation + "void doReinitialisations();")

cppHeaderFile.write("\n" + currentIndentation + "void runInteractions();")

cppHeaderFile.write("\n" + currentIndentation + "void printstatetuple();") 

cppHeaderFile.write("\n" + currentIndentation + "void fireinsttransitiongroup(std::string);")

cppHeaderFile.write("\n" + currentIndentation + "int_fast64_t stateSize() const;")

cppHeaderFile.write("\n" + currentIndentation + "bool figaromodelhasinstransitions();")

cppHeaderFile.write("""
        };
    }
}""")

cppHeaderFile.close() # ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    



    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
