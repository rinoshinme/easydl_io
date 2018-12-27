from xml.etree import ElementTree
from xml.etree.ElementTree import Element, SubElement
from lxml import etree
import codecs

XML_EXT = '.xml'
ENCODE_METHOD = 'utf-8'


class PascalVOCXML:
    def __init__(self):
        # a list of dictionaries of box information
        # xmin, ymin, xmax, ymax, name(label), difficult
        self.boxlist = []
        self.foldername = None
        self.filename = None
        self.database_src = None
        # [h, w, d]
        self.image_size = None
        self.local_image_path = None
        self.verified = False

    def load(self, xml_path):
        self.verified = False
        self.parse_xml(xml_path)

    def save(self, target_file):
        root = self.gen_xml_root()
        self.gen_xml_objects(root)
        if target_file is None:
            out_file = codecs.open(self.filename + XML_EXT, 'w', encoding=ENCODE_METHOD)
        else:
            out_file = codecs.open(target_file, 'w', encoding=ENCODE_METHOD)
        pretty_result = self.prettify(root)
        out_file.write(pretty_result.decode('utf8'))
        out_file.close()

    # methods for generating xml
    def gen_xml_root(self):
        if self.filename is None or self.foldername is None or self.image_size is None:
            return None

        top = Element('annotation')
        if self.verified:
            top.set('verified', 'yes')
        else:
            top.set('verified', 'no')
        folder = SubElement(top, 'folder')
        folder.text = self.foldername
        filename = SubElement(top, 'filename')
        filename.text = self.filename
        if self.local_image_path is not None:
            local_image_path = SubElement(top, 'path')
            local_image_path.text = self.local_image_path

        source = SubElement(top, 'source')
        database = SubElement(source, 'database')
        database.text = self.database_src

        size_part = SubElement(top, 'size')
        width = SubElement(size_part, 'width')
        height = SubElement(size_part, 'height')
        depth = SubElement(size_part, 'depth')
        width.text = str(self.image_size[1])
        height.text = str(self.image_size[0])
        if len(self.image_size) == 3:
            depth.text = str(self.image_size[2])
        else:
            depth.text = '1'

        segmented = SubElement(top, 'segmented')
        segmented.text = '0'

        return top

    def gen_xml_objects(self, top):
        for obj in self.boxlist:
            object_item = SubElement(top, 'object')

            name = SubElement(object_item, 'name')
            name.text = obj['name']

            pose = SubElement(object_item, 'pose')
            pose.text = 'Unspecified'

            truncated = SubElement(object_item, 'truncated')
            if int(obj['ymax']) == int(self.image_size[0]) or int(obj['ymin']) == 1:
                truncated.text = '1'
            elif int(obj['xmax']) == int(self.image_size[1]) or int(obj['xmin']) == 1:
                truncated.text = '1'
            else:
                truncated.text = '0'

            difficult = SubElement(object_item, 'difficult')
            difficult.text = str(bool(obj['difficult']) & 1)

            bndbox = SubElement(object_item, 'bndbox')
            xmin = SubElement(bndbox, 'xmin')
            xmin.text = str(obj['xmin'])
            ymin = SubElement(bndbox, 'ymin')
            ymin.text = str(obj['ymin'])
            xmax = SubElement(bndbox, 'xmax')
            xmax.text = str(obj['xmax'])
            ymax = SubElement(bndbox, 'ymax')
            ymax.text = str(obj['ymax'])

    @staticmethod
    def prettify(elem):
        rough_string = ElementTree.tostring(elem, 'utf8')
        root = etree.fromstring(rough_string)
        return etree.tostring(root, pretty_print=True, encoding=ENCODE_METHOD).replace('  '.encode(), "\t".encode())

    # methods for parsing xml
    def parse_xml(self, xml_path):
        parser = etree.XMLParser(encoding=ENCODE_METHOD)
        xml_tree = ElementTree.parse(xml_path, parser=parser).getroot()

        try:
            verified = xml_tree.attrib['verified']
            if verified == 'yes':
                self.verified = True
        except KeyError:
            self.verified = False

        self.foldername = xml_tree.find('folder').text
        self.filename = xml_tree.find('filename').text

        path = xml_tree.find('path')
        if path is not None:
            self.local_image_path = path.text
        else:
            self.local_image_path = None

        source = xml_tree.find('source')
        self.database_src = source.find('database').text

        size_part = xml_tree.find('size')
        self.image_size = []
        self.image_size.append(int(size_part.find('height').text))
        self.image_size.append(int(size_part.find('width').text))
        self.image_size.append(int(size_part.find('depth').text))

        self.boxlist.clear()
        for obj_elem in xml_tree.findall('object'):
            self.parse_xml_object(obj_elem)

    def parse_xml_object(self, obj_elem):
        bndbox = obj_elem.find('bndbox')
        label = obj_elem.find('name').text
        difficult = False
        xmin = int(bndbox.find('xmin').text)
        ymin = int(bndbox.find('ymin').text)
        xmax = int(bndbox.find('xmax').text)
        ymax = int(bndbox.find('ymax').text)

        obj = dict()
        obj['xmin'] = xmin
        obj['ymin'] = ymin
        obj['xmax'] = xmax
        obj['ymax'] = ymax
        obj['name'] = label
        obj['difficult'] = difficult
        self.boxlist.append(obj)


if __name__ == '__main__':
    xml_file = r'E:/data/fishing/annotations/000010.xml'
    dst_name = r'E:/data/test.xml'
    annot = PascalVOCXML()
    annot.load(xml_file)

    for box in annot.boxlist:
        print(box['name'])

    annot.save(dst_name)
