import numpy as np

# Tamanhos dos arquivos em bytes
file_sizes = [500 * 1024**2, 1024**3, 2 * 1024**3, 4 * 1024**3, 8 * 1024**3]
#[ 1/4 * 2gb , 1/2 * 2gb, 2gb, 4gb, 8gb]

# Diretório onde os arquivos serão salvos
output_directory = "./test_files/"

# Diretório onde os arquivos comprimidos serão salvos
compressed_directory = "./test_files_compressed/"

# Cria o diretório se não existir
import os
os.makedirs(output_directory, exist_ok=True)
os.makedirs(compressed_directory, exist_ok=True)

# Gera e salva os arquivos
for size in file_sizes:
    # Calcula o número de elementos para obter o tamanho desejado
    num_elements = size // np.dtype(np.float64).itemsize

    # Gera números aleatórios com distribuição normal
    random_numbers = np.random.normal(size=num_elements, loc=0, scale=0.3)

    # Redimensiona para o tamanho desejado
    random_numbers = random_numbers[:num_elements]

    # Salva o array no formato padrão do numpy
    file_path = os.path.join(output_directory, f"random_data_{size // 1024**2}MB.npy")
    np.save(file_path, random_numbers)
    print(f"Arquivo salvo em {file_path}")
