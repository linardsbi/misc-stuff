import re, os
Import("env") # access to global build environment

class PATHS:
    route_file = "./src/main.cpp"
    base_path = "./src/"

def after_build(source, target, env):
    env.Execute(f"mv -f {PATHS.route_file}.tmp {PATHS.route_file}")

def find_template_files(text):
    template_params = re.findall('\{\{".*"\}\}', text) # template synax: {{"directory/filename"}}
    paths = [f"{PATHS.base_path}{name[3:-3]}.html" for name in template_params]
    return paths, template_params

def preprocess(source, target, env):
    env.Execute(f"cp {PATHS.route_file} {PATHS.route_file}.tmp")

    with open(PATHS.route_file, "r") as file:
        controller_text = "".join(map(str,file.readlines()))
        paths, template_params = find_template_files(controller_text)

    with open(PATHS.route_file, "w") as file:
        try:
            for i in range(len(paths)):
                with open(paths[i], "r") as html_file:
                    processed_html = "".join(map(str, html_file.readlines()))
                    controller_text = controller_text.replace(str(template_params[i]), 'PSTR(R"raw(' + processed_html + ')raw")', 1)
            file.write(controller_text)

        except Exception as e:
            env.Execute(f"mv -f {PATHS.route_file}.tmp {PATHS.route_file}")
            print(e)
            raise e

env.AddPreAction("$BUILD_DIR/src/main.cpp.o", preprocess)
env.AddPostAction("$BUILD_DIR/src/main.cpp.o", after_build) # change files back


