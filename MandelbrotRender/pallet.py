import matplotlib.pyplot as plt
import numpy as np

# Задаем размер палитры
palette_size = 256

# Получаем палитру magma из matplotlib
cmap = plt.get_cmap('plasma')
palette = cmap(np.linspace(0, 1, palette_size))

# Преобразуем палитру в формат RGB
palette_rgb = (palette[:, :3] * 255).astype(np.uint8)

# Выводим значения RGB в консоль
print("{")
for i in range(palette_size):
    r, g, b = palette_rgb[i]
    print("	{" + f"{r}, {g}, {b}" + "},")
print("};")

plt.figure(figsize=(10, 2))
plt.imshow(palette_rgb.reshape(1, palette_size, 3))
plt.axis('off')
plt.show()
