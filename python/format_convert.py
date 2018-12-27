# convert from pascal voc format to text format
'''
1 text file for 1 image

n #(number of objects)
label_name xmin ymin xmax ymax
label_name xmin ymin xmax ymax

'''
from pascal_voc import PascalVOCXML
import os

def convert_file(xml_file, text_file):
	annot = PascalVOCXML()
	annot.load(xml_file)
	n = len(annot.boxlist)
	with open(text_file, 'w') as writer:
		writer.write('%d\n'%n)
		for i in range(n):
			label = annot.boxlist[i]['name']
			xmin = annot.boxlist[i]['xmin']
			ymin = annot.boxlist[i]['ymin']
			xmax = annot.boxlist[i]['xmax']
			ymax = annot.boxlist[i]['ymax']
			writer.write('%s %d %d %d %d\n'%(label, xmin, ymin, xmax, ymax))

if __name__ == '__main__':
	src_dir = '../../../fishing_data/Annotations'
	dst_dir = '../../../fishing_data/Annotations_text'
	files = os.listdir(src_dir)

	for fname in files:
		print(fname)
		src_file = os.path.join(src_dir, fname)
		base_name = fname.split('.')[0]
		dst_file = os.path.join(dst_dir, base_name + ".txt")
		convert_file(src_file, dst_file)
