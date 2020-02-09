import sys

def decode_movie(file):
    header = file.read(1024)

    decode_header_int = lambda base, length: int.from_bytes(header[base:base+length], "little")
    decode_header_str = lambda base, length: str(header[base:base+length], "ASCII")

    movie_signature = decode_header_int(0x0, 4)
    movie_version = decode_header_int(0x4,4)
    movie_id = decode_header_int(0x8,4)
    movie_frames = decode_header_int(0xC,4)
    movie_rerecord_count = decode_header_int(0x10,4)

    movie_fps = header[0x14]
    movie_controller_count = header[0x15]

    movie_sample_count = decode_header_int(0x18,4)

    movie_start_type = decode_header_int(0x1C, 2)
    controller_flags = decode_header_int(0x20, 4)

    movie_ROM_name = decode_header_str(0xC4, 32)

    movie_ROM_crc = decode_header_int(0xE4, 4)
    movie_ROM_country_code = decode_header_int(0xE8, 2)

    movie_ROM_video_plugin = decode_header_str(0x122, 64)
    movie_ROM_sound_plugin = decode_header_str(0x162, 64)
    movie_ROM_input_plugin = decode_header_str(0x1A2, 64)
    movie_ROM_rsp_plugin = decode_header_str(0x1E2, 64)

    movie_author_name = str(header[0x222:0x300], "UTF-8")
    movie_author_desc = str(header[0x300:0x400], "UTF-8")

    print(f"Movie Signature  : {movie_signature}")
    print(f"Movie Version    : {movie_version}")
    print(f"Movie Id         : {movie_id}")
    print(f"Number of Frames : {movie_frames}")
    print(f"Rerecord Count   : {movie_rerecord_count}")
    print(f"Frames Per Second: {movie_fps}")
    print(f"Controller Count : {movie_controller_count}")
    print(f"Frames Per Second: {movie_fps}")
    print(f"Controller Count : {movie_controller_count}")
    print(f"Sample Count     : {movie_sample_count}")
    print(f"Start Type       : {movie_start_type}")
    print(f"ROM Name         : {movie_ROM_name}")
    print(f"ROM CRC32        : {movie_ROM_crc}")
    print(f"ROM Country      : {movie_ROM_country_code}")
    print(f"ROM Video Plugin : {movie_ROM_video_plugin}")
    print(f"ROM Sound Plugin : {movie_ROM_sound_plugin}")
    print(f"ROM Input Plugin : {movie_ROM_input_plugin}")
    print(f"ROM RSP Plugin   : {movie_ROM_rsp_plugin}")
    print(f"Movie Author     : {movie_author_name}")
    print(f"Movie Description: {movie_author_desc}")

    controller_state_vector = []
    controller_state_number = 0

    tmp = file.read(4)

    while tmp:
        tmp_fixed = [tmp[1], tmp[0], tmp[2], tmp[3]]
        if len(controller_state_vector) == 0 or controller_state_vector[-1][1] != tmp_fixed:
            controller_state_vector.append((controller_state_number, tmp_fixed))
        controller_state_number += 1
        tmp = file.read(4)

    return controller_state_vector

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: ./python3 M64Decode.py FileName.m64")
        sys.exit()

    with open(sys.argv[1], "rb") as tas_file:
        controller_vector = decode_movie(tas_file)

        with open(sys.argv[1] + ".txt", "w") as out_file:
            out_file.write("{\n")
            for ind, vec in controller_vector:
                out_file.write("\t")
                out_file.write(f"{{{ind}, {{0x{vec[0]}, 0x{vec[1]}, 0x{vec[2]}, 0x{vec[3]}}}}},\n")
            out_file.write("};\n")

