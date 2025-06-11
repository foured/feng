import math

def generate_gaussian_weights(sigma):
    radius = math.ceil(2 * sigma)
    size = 2 * radius + 1
    kernel = []

    for i in range(-radius, radius + 1):
        weight = math.exp(-(i ** 2) / (2 * sigma ** 2))
        kernel.append(weight)

    # Нормализация
    total = sum(kernel)
    kernel = [w / total for w in kernel]

    radius_index = len(kernel) // 2
    weights = [kernel[radius_index]]  # центр

    # Добавляем только положительную сторону
    for i in range(1, radius + 1):
        weights.append(kernel[radius_index + i])

    count = len(weights)
    print(f"const float weights_{size}[{count}] = float[{count}](")
    for i in range(count):
        comma = ',' if i < count - 1 else ''
        print(f"    {weights[i]:.8f}{comma}")
    print(");")

if __name__ == "__main__":
    sigma = float(input("Введите σ (сигму): "))
    generate_gaussian_weights(sigma)
