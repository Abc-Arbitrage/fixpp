import sys
import xml.etree.ElementTree as ET

TAG_TYPES = {
    'char': 'Char',
    'boolean': 'Boolean',
    'data': 'Data',
    'float': 'Float',
    'qty': 'Qty',
    'amt': 'Amt',
    'price': 'Price',
    'priceoffset': 'Float',
    'int': 'Int',
    'dayofmonth': 'DayOfMonth',
    'length': 'Int',
    'string': 'String',
    'currency': 'String',
    'exchange': 'String',
    'multiplevaluestring': 'MultipleValueString',
    'localmktdate': 'LocalMktDate',
    'monthyear': 'MonthYear',
    'utcdate': 'UTCDate',
    'utctimeonly': 'UTCTimeOnly',
    'utctimestamp': 'UTCTimestamp'
}


class Tag(object):
    def __init__(self, number, name, tag_type):
        self.number = number
        self.name = name
        self.type = self._normalize_type(tag_type)

    def cpp_definition(self):
        return 'using {name} = TagT<{number}, Type::{type}>;'.format(name=self.name, number=self.number, type=self.type)

    def _normalize_type(self, tag_type):
        return TAG_TYPES[tag_type.lower()]

    def __eq__(self, other):
        return self.number == other.number and self.name == other.name

    def __lt__(self, other):
        return self.number < other.number and self.name < other.name


tags_map = dict()
tags = set()

def as_bool(required):
    assert required == 'Y' or required == 'N'

    if required == 'Y':
        return True
    return False

def format_tag(tag_name, required):
    if required:
        return 'Required<Tag::{tag_name}>'.format(tag_name=tag_name)

    return 'Tag::' + tag_name

def parse_fields(root):
    fields_root = root.find('fields')
    for field in fields_root:
        number = field.get('number')
        name = field.get('name')
        tag_type = field.get('type')

        tags_map[name] = Tag(int(number), name, tag_type.lower())

def parse_header(root):
    header_root = root.find('header')

    header_str = 'using Header = MessageBase<Field, '

    ignored_fields = ['BeginString', 'BodyLength', 'MsgType']

    for i, field in enumerate(header_root):
        name = field.get('name')
        if name in ignored_fields:
            continue

        required = as_bool(field.get('required'))

        header_str += format_tag(name, required)
        tags.add(tags_map[name])
        if i < len(header_root) - 1:
            header_str += ', '

    header_str += '>;'
    return header_str

def parse_message_fields(root):
    fields_str = ''

    for i, field in enumerate(root):
        name = field.get('name')
        required = as_bool(field.get('required'))
        tags.add(tags_map[name])

        if field.tag == 'group':
            if required:
                fields_str += 'Required<'
            fields_str += 'RepeatingGroup<' + format_tag(name, False) + ', '
            fields_str += parse_message_fields(field)
            fields_str += '>'
            if required:
                fields_str += '>'
        else:
            fields_str += format_tag(name, required)

        if i < len(root) - 1:
            fields_str += ', '

    return fields_str

def parse_message(message_root):
    msg_name = message_root.get('name')
    msg_type = message_root.get('msgtype')

    msg_str = 'using {MsgName} = MessageV<Chars<\'{MsgType}\'>, '.format(MsgName=msg_name, MsgType=msg_type)
    msg_str += parse_message_fields(message_root)
    msg_str += '>;'

    return msg_str

def format_file(fix_version, name):
    return '{fix_version}.{name}.txt'.format(fix_version=fix_version, name=name)


def parse_xml(xml_file):
    tree = ET.parse(xml_file)
    root = tree.getroot()

    major = root.get('major')
    minor = root.get('minor')

    fix_version = 'FIX.{major}.{minor}'.format(major=major, minor=minor)

    header_file = format_file(fix_version, 'header')
    messages_file = format_file(fix_version, 'messages')
    tags_file = format_file(fix_version, 'tags')

    parse_fields(root)

    header = parse_header(root)
    print 'Writing header to {file}'.format(file=header_file)
    with open(header_file, 'w') as hf:
        hf.write(header)

    print 'Writing messages to {file}'.format(file=messages_file)
    with open(messages_file, 'w') as mf:
        messages = root.find('messages')
        for message_root in messages:
            mf.write(parse_message(message_root))
            mf.write('\n')

    print 'Writing tags to {file}'.format(file=tags_file)
    with open(tags_file, 'w') as tf:
        for tag in sorted(list(tags), key = lambda tag: tag.number):
            tf.write(tag.cpp_definition())
            tf.write('\n')

def main():
    if len(sys.argv) != 2:
        print 'Usage: fixpp.py spec_file.xml'
        return 1

    xml_file = sys.argv[1]
    parse_xml(xml_file)
    return 0

if __name__ == '__main__':
    sys.exit(main())
