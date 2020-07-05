import requests
import sys, os, shutil, time
import pytesseract
from io import BytesIO
from PIL import Image
from PyPDF2 import PdfFileMerger


# 1. download img
# 2. ocr image
# 3. add to text file
# 4. if no more images, save file

DOC_START = 49

def get_image(img_name, img_num):
    print(f"Getting: {img_name} - {img_num}.jpg")
    url = f'http://www.biblioteka.pirtsskola.lv/kvali/{img_name}/{img_num}.jpg'
    r = requests.get(url)

    assert r.status_code == 200, "end of pdf"
    return Image.open(BytesIO(r.content))
    

def ocr_image(image):
    print("- Doing OCR -")
    text = str(((pytesseract.image_to_string(image, lang='lav')))) 
    text = text.replace('-\n', '')

    return text

def save_to_file(item, name, index, ftype):
    filename = f"./{name}/{index}.{ftype}"
    print(f'Saving: {filename}')
    os.makedirs(f"./{name}", exist_ok=True)
    with open(filename, "w+b" if (ftype == "pdf") else "w") as file:
        if (ftype == "pdf"):
            item.write(file)
        else:
            file.write(item)


def download_to_txt(name):
    savable = ""
    i = 1
    try:
        while(True):
            savable += ocr_image(get_image(img_name=name, img_num=i)) + "\n"
            i += 1
    except AssertionError as e:
        print(f'Reached {e}')
        if(savable): 
            save_to_file(item=savable, name=name, index=i, ftype="txt")
        else: 
            print("Empty file")
    finally:
        pass

def write_temp(item, index):
    os.makedirs(f"./tmp", exist_ok=True)
    file = open(f"./tmp/tmp{index}", "wb+")
    file.write(item)
    return file
    

def download_to_pdf(name):
    merger = PdfFileMerger()
    i = 1
    try:
        while(True):
            file = write_temp(pytesseract.image_to_pdf_or_hocr(get_image(img_name=name, img_num=i), extension='pdf', lang='lav'), i)
            merger.append(file)
            i += 1
    except AssertionError as e:
        print(f'Reached {e}')
        save_to_file(item=merger, name=name, index=i, ftype="pdf")
    else:
        shutil.rmtree(f'./{name}')
    finally:
        shutil.rmtree('./tmp')

start = time.time()
line_count = 0
with open("input.txt", "r") as file:
    files = [a for a in os.listdir()]
    
    for line in file.readlines():
        if (line[DOC_START:-1] in files): 
                print(f"Skipping: {line[DOC_START:-1]}")
                continue

        if ('--pdf' in sys.argv):
            download_to_pdf(line[DOC_START:-1])
        else:
            download_to_txt(line[DOC_START:-1])

        line_count += 1

print(f"Finished, elapsed time for {line_count} items: {time.time() - start} seconds")
        

 