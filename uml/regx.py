import re
import os
path = '/home/huiyeruzhou/Documents/WorkSpace/embeeded-rpc/embeeded-rpc/uml/header/'
f = os.listdir(path)[0]
f = open(path + f, 'r')
string = f.read()

pattern = r'\/\*[\s\S\n]*?\*\/|\/\/.*'
repl = ' '
string = re.sub(pattern, repl, string, count=0, flags=0)

pattern = r'\n([ \t]*\n)+'
repl = r'\n'
string = re.sub(pattern, repl, string, count=0, flags=0)

pattern = r'\)[\s(const)]*\{([\s\S\n]*?;)*?\s*?\}'
repl = r')'
string = re.sub(pattern, repl, string, count=0, flags=0)

pattern = r'virtual[\s]*([^\n]*?\([ \S]*?\)[\s]*?)=[\s]*0[\s]*;'
repl = r'{abstract}'

def abstract_repl(matched):
    value = matched.group(1)
    return repl + str(value)
string = re.sub(pattern, abstract_repl, string, count=0, flags=0)

print(string)

f = open('/home/huiyeruzhou/Documents/WorkSpace/embeeded-rpc/embeeded-rpc/uml/output.hpp', 'w+')
f.write(string)



