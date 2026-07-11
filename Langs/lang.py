import configparser
import sys

# Ensure stdout uses UTF-8 so Turkish characters print correctly when module is imported
try:
    sys.stdout.reconfigure(encoding='utf-8')
except Exception:
    pass

lang_config = configparser.ConfigParser()
lang_config.read('Lang/lang.ini', encoding='utf-8-sig')
lang = lang_config['Lang']['lang']
lang_available = lang_config.getboolean('Lang', 'lang_available')
langcfg = configparser.ConfigParser()

if not lang_available:
    print("No language has been set in the lang.ini file; please select the language you want to use in this script.")
    lang_selection = str(input("Please select the language you want to use while running this script (en / tr): ")).strip().lower()
    if lang_selection not in ("en", "tr"):
        print("Invalid language!")
        exit()
    else:
        lang_config['Lang']['lang_available'] = str(True)
        lang_config['Lang']['lang'] = lang_selection
        lang = lang_selection
        with open('Lang/lang.ini', 'w', encoding='utf-8') as f:
            lang_config.write(f)

# Load the selected language file and validate it
langcfg = configparser.ConfigParser()
langcfg_file = langcfg.read(f'Lang/{lang}.ini', encoding='utf-8-sig')

if not langcfg_file:
    print(f'The {lang}.ini file was not found. Please make sure the files in the Lang folder are present.')
    exit(1)
# Ensure the expected section exists before accessing it
if 'Content' not in langcfg:
    print(f"The {lang}.ini file does not contain a 'Content' section. Please add '[Content]' and retry.")
    exit(1)

fs_unzip_content = langcfg['Content']['fs_unzip_content']
unzip_not_found_error = langcfg['Content']['unzip_not_found_error']
unzip_not_setup_error = langcfg['Content']['unzip_not_setup_error']
unzip_not_success_error = langcfg['Content']['unzip_not_success_error']
fs_success = langcfg['Content']['fs_success']
aarch64_gcc_not_found_error = langcfg['Content']['aarch64_gcc_not_found_error']
gcc_not_setup_error = langcfg['Content']['gcc_not_setup_error']
pati_commands_file_not_found_error = langcfg['Content']['pati_commands_file_not_found_error']
compile_success = langcfg['Content']['compile_success']
compile_not_success = langcfg['Content']['compile_not_success']
compilation_complete = langcfg['Content']['compilation_complete']
success = langcfg['Content']['success']
incorrect = langcfg['Content']['incorrect']
create_dev_nodes = langcfg['Content']['create_dev_nodes']
create_initramfs = langcfg['Content']['create_initramfs']
initramfs_ready = langcfg['Content']['initramfs_ready']
not_create_initramfs = langcfg['Content']['not_create_initramfs']
moved = langcfg['Content']['moved']