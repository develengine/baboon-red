from sys import argv
from struct import pack

# python3 objconverter.py jammer/res/neofrog.obj:jammer/res/frog2.png jammer/res/lilypad.obj:jammer/res/lilypad.png jammer/res/rock.obj:jammer/res/rock.png jammer/res/shadow.obj:jammer/res/shadow.png -o jammer/res/models.bin

class Model:
    def __init__(self, name):
        self.name = name
        self.vertices = [ ]
        self.textures = [ ]
        self.normals = [ ]
        self.faces = [ ]
        self.texture = None

def main():
    output_dir = 'output.bin'
    input_dirs = [ ]

    i = 1
    while i < len(argv):
        arg = argv[i]
        if arg == '-o' or arg == '--output':
            output_dir = argv[i + 1]
            i += 2
        else:
            mod_tex = arg.split(':')
            input_dirs.append(mod_tex)
            i += 1

    models = [ ]

    for d in input_dirs:
        if d == '':
            continue

        input_file = open(d[0], 'r')
    
        for i in input_file.readlines():
            words = i.split()
    
            if words[0] == 'o':
                models.append(Model(words[1]))
                models[-1].texture = d[1]
            elif words[0] == 'v':
                models[-1].vertices.append(tuple(map(float, words[1:])))
            elif words[0] == 'vt':
                models[-1].textures.append(tuple(map(float, words[1:])))
            elif words[0] == 'vn':
                models[-1].normals.append(tuple(map(float, words[1:])))
            elif words[0] == 'f':
                models[-1].faces.append(tuple(map(lambda x: tuple(map(int, x.split('/'))), words[1:])))
    
        input_file.close()


    endiannes = 'big'
    model_symbol  = 7
    vertex_symbol = 42
    index_symbol  = 69
    texture_symbol = 96;

    to_u8  = lambda num: num.to_bytes(1, byteorder = endiannes, signed = False)
    to_float = lambda num: pack(('>' if endiannes == 'big' else '<') + 'f', num)
    to_uint  = lambda num: num.to_bytes(4, byteorder = endiannes, signed = False)

    output_file = open(output_dir, 'wb')

    output_file.write(to_float(6.66))
    output_file.write(to_uint(666))
    output_file.write(to_uint(len(models)))

    for model in models:
        output_file.write(to_u8(model_symbol))
        output_file.write(str.encode(model.name))
        output_file.write(to_u8(0))

        output_file.write(to_u8(texture_symbol))
        output_file.write(str.encode(model.texture))
        output_file.write(to_u8(0))

        vertex_count = 0
        output_indices  = [ ]
        output_vertices = [ ]
        created_indices = { }

        for face in model.faces:
            for vertex in face:
                index = created_indices.get(vertex)

                if index == None:
                    output_vertices += [
                        *model.vertices[vertex[0] - 1],
                        *model.normals[vertex[2] - 1],
                        *model.textures[vertex[1] - 1]
                    ]

                    output_indices.append(vertex_count)
                    vertex_count += 1
                    created_indices[vertex] = index
                else:
                    output_indices.append(index)

        output_file.write(to_u8(vertex_symbol))
        output_file.write(to_uint(len(output_vertices) * 4))
        for i in output_vertices:
            output_file.write(to_float(i))

        output_file.write(to_u8(index_symbol))
        output_file.write(to_uint(len(output_indices) * 4))
        for i in output_indices:
            output_file.write(to_uint(i))

    output_file.close()

main()

