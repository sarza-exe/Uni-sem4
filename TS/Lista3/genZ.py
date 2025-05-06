import random

with open("z.txt", "w") as file:
    content = ''.join(random.choice(['0', '1']) for _ in range(10000))
    file.write(content)
