import subprocess

def generate_file_name(param2, param3, param4, param5):
    # Pad with leading zeros if the number is less than 10 (for lex. ordering)
    param2_str = str(param2).zfill(2)
    param3_str = str(param3).zfill(2)
    param4_str = str(param4).zfill(2)

    file_name = f"{param2_str}_{param3_str}_{param4_str}_{param5}.qasm"
    return file_name

def call_generator_script(param2, param3, param4, param5):
    file_name = generate_file_name(param2, param3, param4, param5)

    script_name = "generator_QC.py"
    command = [
        "python",
        script_name,
        file_name,
        str(param2),
        str(param3),
        str(param4),
        str(param5)
    ]

    subprocess.run(command)

if __name__ == "__main__":
    param2_values = list(range(7, 16))
    param4_values = [5, 10, 15]
    param5_values = [0]

    for param2 in param2_values:
        for param4 in param4_values:
            for param5 in param5_values:
                call_generator_script(param2, param2 // 2, param4, param5)