import ast

class Preprocessor(ast.NodeTransformer):
    def __init__(self):
        super().__init__()
        # Initialize with an empty target name
        self.target_name = ""
        self.symbol_table = {}  # Tracks variable assignments
        self.function_sizes = {}  # Tracks the maximum size for each function argument

    # for-range statements such as:
    #
    #   for x in range(1, 5, 1):
    #     print(x)
    #
    # are transformed into a corresponding while loop with the following structure:
    #
    #   def ESBMC_range_next_(curr: int, step: int) -> int:
    #     return curr + step
    #
    #   def ESBMC_range_has_next_(curr: int, end: int, step: int) -> bool:
    #     return curr + step <= end
    #
    #   start = 1  # start value is copied from the first parameter of range call
    #   has_next:bool = ESBMC_range_has_next_(1, 5, 1) # ESBMC_range_has_next_ parameters copied from range call
    #   while has_next == True:
    #     print(start)
    #   start = ESBMC_range_next_(start, 1)
    #   has_next = ESBMC_range_has_next_(start, 5, 1)

    def visit_For(self, node):
        # Transformation from for to while if the iterator is range
        if isinstance(node.iter, ast.Call) and isinstance(node.iter.func, ast.Name) and node.iter.func.id == "range":

            if len(node.iter.args) > 1:
                start = node.iter.args[0]  # Start of the range
                end = node.iter.args[1]    # End of the range
            elif len(node.iter.args) == 1:
                start = ast.Constant(value=0)
                end = node.iter.args[0]

            # Check if step is provided in range, otherwise default to 1
            if len(node.iter.args) > 2:
                step = node.iter.args[2]
            else:
                step = ast.Constant(value=1)

            # Create assignment for the start variable
            start_assign = ast.AnnAssign(
                target=ast.Name(id='start', ctx=ast.Store()),
                annotation=ast.Name(id='int', ctx=ast.Load()),
                value=start,
                simple=1
            )

            # Create call to ESBMC_range_has_next_ function for the range
            has_next_call = ast.Call(
                func=ast.Name(id='ESBMC_range_has_next_', ctx=ast.Load()),
                args=[start, end, step],
                keywords=[]
            )

            # Create assignment for the has_next variable
            has_next_assign = ast.AnnAssign(
                target=ast.Name(id='has_next', ctx=ast.Store()),
                annotation=ast.Name(id='bool', ctx=ast.Load()),
                value=has_next_call,
                simple=1
            )

            # Create condition for the while loop
            has_next_name = ast.Name(id='has_next', ctx=ast.Load())
            while_cond = ast.Compare(
                left=has_next_name,
                ops=[ast.Eq()],
                comparators=[ast.Constant(value=True)]
            )

            # Transform the body of the for loop
            transformed_body = []
            self.target_name = node.target.id # Store the target variable name for replacement
            for statement in node.body:
                transformed_body.append(self.visit(statement))

            # Create the body of the while loop, including updating the start and has_next variables
            while_body = transformed_body + [
                ast.Assign(
                    targets=[ast.Name(id='start', ctx=ast.Store())],
                    value=ast.Call(
                        func=ast.Name(id='ESBMC_range_next_', ctx=ast.Load()),
                        args=[ast.Name(id='start', ctx=ast.Load()), step],
                        keywords=[]
                    )
                ),
                ast.Assign(
                    targets=[has_next_name],
                    value=ast.Call(
                        func=ast.Name(id='ESBMC_range_has_next_', ctx=ast.Load()),
                        args=[ast.Name(id='start', ctx=ast.Load()), end, step],
                        keywords=[]
                    )
                )
            ]

            # Create the while statement
            while_stmt = ast.While(
                test=while_cond,
                body=while_body,
                orelse=[]
            )

            # Return the transformed statements
            return [start_assign, has_next_assign, while_stmt]

        return node


    def visit_Name(self, node):
        # Replace variable names as needed in the for to while transformation
        if node.id == self.target_name:
            node.id = 'start'  # Replace the variable name with 'start'
        return node


    def visit_Assign(self, node):
        # Track variable assignments
        if isinstance(node.targets[0], ast.Name):
            var_name = node.targets[0].id
            if isinstance(node.value, ast.Constant) and isinstance(node.value.value, int):
                self.symbol_table[var_name] = node.value.value
        return self.generic_visit(node)


    def visit_Call(self, node):
        # Transformation for int.from_bytes calls
        if isinstance(node.func, ast.Attribute) and isinstance(node.func.value, ast.Name) and node.func.value.id == "int" and node.func.attr == "from_bytes":
            # Replace 'big' argument with True and anything else with False
            if len(node.args) > 1 and isinstance(node.args[1], ast.Str) and node.args[1].s == 'big':
                node.args[1] = ast.NameConstant(value=True)
            else:
                node.args[1] = ast.NameConstant(value=False)


        # Handle large int arguments passed as constants or variables
        if isinstance(node, ast.Call):
            # Extract function name
            func_name = node.func.id if isinstance(node.func, ast.Name) else None

            if func_name:
                for arg in node.args:
                    value = None
                    if isinstance(arg, ast.Constant) and isinstance(arg.value, int):
                        value = arg.value
                    elif isinstance(arg, ast.Name) and arg.id in self.symbol_table:
                        value = self.symbol_table[arg.id]

                    if value is not None and value > 0xFFFFFFFF:
                        # Record the maximum size required for the function
                        max_size = value.bit_length() # The size in bits required for the value
                        self.function_sizes[func_name] = max(
                            self.function_sizes.get(func_name, 0), max_size
                        )

        return self.generic_visit(node)


    def visit_FunctionDef(self, node):
        # Process function arguments and update annotations
        for arg in node.args.args:
            if isinstance(arg.annotation, ast.Name) and arg.annotation.id == "int":
                # Default to 32 bits if no calls are tracked
                max_size = self.function_sizes.get(node.name, 32)
                # Update annotation with size field
                arg.annotation.size = max_size
        return self.generic_visit(node)


    def visit_Module(self, node):
        # First, visit assignments
        for child in node.body:
            if isinstance(child, ast.Assign):
                self.visit(child)

        # Then, visit function calls
        for child in node.body:
            if isinstance(child, ast.Expr) and isinstance(child.value, ast.Call):
                self.visit(child)

        # Finally, visit function definitions
        for child in node.body:
            if isinstance(child, ast.FunctionDef):
                self.visit(child)

        # Ensure other node types (like For) are visited
        self.generic_visit(node)

        return node
