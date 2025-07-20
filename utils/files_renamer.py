import os

def change_extensions(directory, old_ext, new_ext):
    if not new_ext.startswith('.'):
        new_ext = '.' + new_ext

    for filename in os.listdir(directory):
        if filename.lower().endswith(old_ext.lower()):
            base = os.path.splitext(filename)[0]
            new_name = base + new_ext
            old_path = os.path.join(directory, filename)
            new_path = os.path.join(directory, new_name)
            os.rename(old_path, new_path)
            print(f'Переименован: {filename} -> {new_name}')

if __name__ == '__main__':
    print(f'Текущий путь: {os.getcwd()}')
    folder = input('Укажите путь к дериктории: ').strip()
    old_extension = input('Старое расширение (без точки):').strip()
    new_extension = input('Новое расширение (без точки):').strip()
    change_extensions(folder, '.' + old_extension, '.' + new_extension)
