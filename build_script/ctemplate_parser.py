from __future__ import print_function
import sys

from pycparser import c_parser, c_ast, parse_file




class Template_FuncCall:

    def __init__(self, name, line_num):
        self.name = name
        self.line_num = line_num
        self.args = []
        self.extra_data = []

    def add_argument(self, string, t):
        if t is None:
            raise Exception #TODO
        self.args.append((string, t))

    def get_arg_type(self, arg):
        return arg[1]
    def get_arg_name(self, arg):
        return arg[0]

    def has_extra(self, n):
        if n in self.extra_data:
            return True
        return False

def get_name(node):
    t = type(node)
    if t is c_ast.Struct:
        if node.name is None: #unnamed struct, probably typedeffed
            return None
        else:
            get_name(node.name)
    elif t is c_ast.RFTemplateKeyword:
        return get_name(node.name)
    elif t is c_ast.ID:
        return get_name(node.name)
    elif t is c_ast.StructRef:
        return get_name(node.name)
    elif t is str:
        return node
    else:
        raise Exception #TODO

class Analyzer:
    def __init__(self, parents):
        self.parents = parents
        self.index = len(parents) - 1


    def recurse_member_search(self, struct_name, member_name, node):
        try:
            children = node.children()
        except:
            return
        for (t, child) in children:
            if ((type(child) is c_ast.Struct and get_name(child) == struct_name) or
                (type(child) is c_ast.Struct and
                 type(node) is c_ast.TypeDecl and
                 node.declname == struct_name
             )):
                for decl in child.decls:
                    if decl.name == member_name:
                        return self.find_type_r(member_name, decl)
            t = self.recurse_member_search(struct_name, member_name, child)
            if not t:
                continue
            else:
                return t

    def find_type_of_member(self, struct_name, member_name):
        for i, node in reversed(list(enumerate(self.parents))):
            t = self.recurse_member_search(struct_name, member_name, node)
            if t is not None:
                return t



    def find_struct_member_type(self, left_node, right_node):
        struct_name = self.find_type(left_node.name)
        # remove any pointer marks from the name
        struct_name = struct_name.replace('*', '')
        return self.find_type_of_member(struct_name, right_node.name)





    def find_type_r(self, ID, node):
        if type(node) is c_ast.FuncDef:
            for param in node.decl.type.args.params:
                t = self.find_type_r(ID, param)
                if t is not None:
                    return t
        elif type(node) is c_ast.Compound:
            for c in node.block_items:
                t = self.find_type_r(ID, c)
                if t is not None:
                    return t
        elif type(node) is c_ast.Decl:
            if node.name == ID: # if name matches
                return self.find_type_r(ID, node.type)
        elif type(node) is c_ast.TypeDecl:
            if node.declname == ID: # if name matches
                return self.find_type_r(ID, node.type)
        elif type(node) is c_ast.IdentifierType:
            return node.names[0]
        elif type(node) is c_ast.RFTemplateKeyword:
            return node.name
        elif type(node) is c_ast.StructRef:
            return self.find_struct_member_type(node.name, node.field)
        elif type(node) is c_ast.PtrDecl:
            # for now no special pointer handling. Just add the asterisk in the
            # type string
            return self.find_type_r(ID, node.type) + "*"
        elif type(node) is c_ast.BinaryOp:
            # for now just take the type of left side of operation
            # irrespective of op type
            return self.find_type_r(ID, node.left)



    def find_type(self, ID):
        for i, node in reversed(list(enumerate(self.parents))):
            t = self.find_type_r(ID, node)
            if t is not None:
                return t



    def determine_argument(self, node):
        if type(node) is c_ast.BinaryOp:
            (lefts, _) = self.determine_argument(node.left)
            (rights, _) = self.determine_argument(node.right)
            return (
                "{} {} {}".format(lefts, node.op, rights),
                # just get the type of the left side
                self.find_type(get_name(node.left))
            )
        elif type(node) is c_ast.UnaryOp:
            s = self.determine_argument(node.expr)
            return (
                "{}{}".format(node.op, s),
                self.find_type(get_name(node.expr))
            )
        elif type(node) is c_ast.StructRef:
            (sname, _) = self.determine_argument(node.name)
            (fname, _) = self.determine_argument(node.field)
            return (
                "{}{}{}".format(sname, node.type, fname),
                self.find_struct_member_type(node.name, node.field)
            )
        elif type(node) is c_ast.ID:
            return (node.name, self.find_type(node.name))
        elif type(node) is c_ast.Constant:
            return (node.value, node.type)
        else:
            raise Exception #TODO (Unhandled type argument)

class Visitor(c_ast.NodeVisitor):

    def __init__(self, p):
        self.template_obj = p
        self.fc_dict = {
            "@OBJECT_COPY": self.handle_object_copy,
            "@OBJECT_DESTROY": self.handle_object_destroy,
            "@OBJECT_COMPARE": self.handle_object_compare,
            "@ASSIGN": self.handle_assign,
            "@SHALLOW_PTR_ARITHMETIC": self.handle_shallow_ptr_arithmetic,
        }


    def handle_object_copy(self, node, parents):
        args = node.args.exprs
        if len(node.args.exprs) != 2:
            raise Exception #TODO
        call = Template_FuncCall("@OBJECT_COPY", node.coord.line)
        analyzer = Analyzer(parents)

        for arg in node.args.exprs:
            (s, t) = analyzer.determine_argument(arg)
            call.add_argument(s, t)

        self.template_obj.add_tfunc_call(call)
    def handle_object_compare(self, node, parents):
        args = node.args.exprs
        if len(node.args.exprs) != 2:
            raise Exception #TODO

        call = Template_FuncCall("@OBJECT_COMPARE", node.coord.line)
        analyzer = Analyzer(parents)
        for arg in node.args.exprs:
            (s, t) = analyzer.determine_argument(arg)
            call.add_argument(s, t)

        self.template_obj.add_tfunc_call(call)

    def handle_object_destroy(self, node, parents):
        args = node.args.exprs
        if len(node.args.exprs) != 1:
            raise Exception #TODO
        analyzer = Analyzer(parents)
        (s, t) = analyzer.determine_argument(node.args.exprs[0])
        call = Template_FuncCall("@OBJECT_DESTROY", node.coord.line)
        call.add_argument(s, t)

        self.template_obj.add_tfunc_call(call)

    def handle_assign(self, node, parents):
        args = node.args.exprs
        if len(node.args.exprs) != 2:
            raise Exception #TODO
        analyzer = Analyzer(parents)
        call = Template_FuncCall("@ASSIGN", node.coord.line)
        for arg in node.args.exprs:
            (s, t) = analyzer.determine_argument(arg)
            call.add_argument(s, t)
        self.template_obj.add_tfunc_call(call)

    def handle_shallow_ptr_arithmetic(self, node, parents):
        args = node.args.exprs
        if len(node.args.exprs) != 1:
            raise Exception #TODO
        analyzer = Analyzer(parents)
        (s, t) = analyzer.determine_argument(node.args.exprs[0])
        call = Template_FuncCall("@SHALLOW_PTR_ARITHMETIC", node.coord.line)
        call.add_argument(s, t)

        # Not the best way to determine this, but by checking
        # if the call is inside a return then we can dereference
        # for POD ... if possible improve the method in the future
        if type(parents[-1]) is c_ast.Return:
            call.extra_data.append("IN_RETURN")

        self.template_obj.add_tfunc_call(call)



    def handle_TemplateFunctionCall(self, node, parents):
        if node.name.name in self.fc_dict:
            self.fc_dict[node.name.name](node, parents)

    def handle_TemplateKeyword(self, node, parents):
        """ Don't need anything for them for now """
        pass



#-- The following functions read all of the ast and do stuff for each
#-- object in the handler dictionary
    def visit_FileAST(self, node):
            parent_chain = [node]
            self.recurse_stmts(node, parent_chain)

    def act_on_node(self, node, parents):
        if isinstance(node, c_ast.RFTemplateFuncCall):
            self.handle_TemplateFunctionCall(node, parents)
        elif isinstance(node, c_ast.RFTemplateKeyword):
            self.handle_TemplateKeyword(node, parents)

    def recurse_stmts(self, node, parent_chain):
        try:
            children = node.children()
        except:
            return
        for (name, child) in children:
            self.act_on_node(child, parent_chain)
            new_parent_chain = parent_chain[:]
            new_parent_chain.append(child)
            self.recurse_stmts(child, new_parent_chain)

class TemplateParser:


    def query(self, func_call_name, line_num):
        for f in self.template_func_calls:
            if f.name == func_call_name and line_num == f.line_num:
                return f

    def add_tfunc_call(self, func_callobj):
        self.template_func_calls.append(func_callobj)

    def __init__(self, filename, incl_dirs, extra_dirs):
        self.template_func_calls = []
        self.filename = filename
        cpp_args=[
            r'-Ipycparser/utils/fake_libc_include',

            r'-D__attribute__(x)= ',
            r'-D__restrict= ',
            r'-D__thread= ',
            r'-D__builtin_va_list=int',
            r'-D__asm__(x)= ',
            r'-D__inline= ',
            r'-D__extension__= ',
        ]

        if incl_dirs is not None:
            for incl in incl_dirs:
                cpp_args.append(r'-I{}'.format(incl))

        if extra_dirs is not None:
            for incl in extra_dirs:
                cpp_args.append(r'-I{}'.format(incl))

        self.ast = parse_file(filename, use_cpp=True,
                              cpp_args=cpp_args)

    def process(self):
        v = Visitor(self)
        v.visit(self.ast)
