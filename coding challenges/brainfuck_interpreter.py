def interpret(bf_input):
    array = [0] * 30000
    index = 0
    print_buffer = ""

    # used for jumps; no need to search for opening [
    loop_start_indexes = []

    loop_index = 0
    while True:
        if loop_index >= len(bf_input):
            yield print_buffer
            break

        char = bf_input[loop_index]

        if char == ">":
            index += 1
        elif char == "<":
            index -= 1
        elif char == "+":
            array[index] += 1
        elif char == "-":
            array[index] -= 1
        elif char == ".":
            print_buffer += chr(array[index])
        elif char == ",":
            if char := input():
                array[index] = ord(char)
        elif char == "[":
            if not loop_start_indexes or loop_start_indexes[-1] != loop_index:
                loop_start_indexes.append(loop_index)
                
            if array[index] == 0:
                # find closing ] by searching ahead
                i = loop_index + 1
                brace_count = 1
                while brace_count > 0:
                    if bf_input[i] == "[":
                        brace_count += 1
                    elif bf_input[i] == "]":
                        brace_count -= 1
                    i += 1
                loop_index = i

                loop_start_indexes.pop()
                continue
        elif char == "]":
            if array[index] != 0:
                loop_index = loop_start_indexes[-1]
                continue
            else:
                loop_start_indexes.pop()

        loop_index += 1
        
        if print_buffer and print_buffer[-1] == "\n" or len(print_buffer) > 300:
            yield print_buffer
            print_buffer = ""


# parse test: should print H\n
#gen = interpret(r'[]++++++++++[>>+>+>++++++[<<+<+++>>>-]<<<<-]"A*$";?@![#>>+<<]>[>>]<<<<[>++<[-]]>.>.')

# print hello world
# gen = interpret("++++++++[>++++[>++>+++>+++>+<<<<-]>+>+>->>+[<]<-]>>.>---.+++++++..+++.>>.<-.<.+++.------.--------.>>+.>++.")

# reverse input
# gen = interpret(">,[>,]<[.<]")

# output powers of two
gen = interpret(">++++++++++>>+<+[[+++++[>++++++++<-]>.<++++++[>--------<-]+<<]>.>[->[<++>-[<++>-[<++>-[<++>-[<-------->>[-]++<-[<++>-]]]]]]<[>+<-]+>>]<<]")

# random ascii generation
# gen = interpret(">>++>+<[[>>]+>>+[-[++++++[>+++++++>+<<-]>-.>[<------>-]++<<]<[>>[-]]>[>[-<<]+<[<+<]]+<<]>>]")

for item in gen:
    print(item)

