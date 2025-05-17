import math

def generate_gamma_lut(gamma, max_value=255, resolution=256):
    lut = [round((x / (resolution - 1)) ** gamma * max_value) for x in range(resolution)]
    return lut

if __name__ == "__main__":
    gamma = 2.2
    lut = generate_gamma_lut(gamma)

    # Imprimi-la LUT como código C
    print("static const uint8_t gammaLUT[256] = {")
    print(", ".join(map(str, lut)))
    print("};")


