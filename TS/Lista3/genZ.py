import random

with open("z.txt", "w") as file:
    content = ''.join(random.choice(['0', '1']) for _ in range(1300))
    file.write(content)
