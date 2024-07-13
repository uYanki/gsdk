import os
import chardet

path = "."


for root, dirs, files in os.walk(path):
    
    for file in files:
        
        if file.endswith(".h") or file.endswith(".c"):
            
            filepath = os.path.join(root, file)

            f = open(filepath, 'rb')
            result = chardet.detect(f.read())
            f.close()

            if result['encoding'] == "GB2312":

                print(filepath)

                f = open(filepath, 'r', encoding=result['encoding'], errors='replace')
                text = f.read()
                f.close()
                f = open(filepath, 'w', encoding='utf-8', errors='replace')
                f.write(text)
                f.close()

print("ok")
