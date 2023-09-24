import os
Import("env")

# include toolchain paths
env.Replace(COMPILATIONDB_INCLUDE_TOOLCHAIN=True)

# override compilation DB path
pwd = os.getcwd()
env.Replace(COMPILATIONDB_PATH=os.path.join(pwd, ".vscode", "compile_commands.json"))