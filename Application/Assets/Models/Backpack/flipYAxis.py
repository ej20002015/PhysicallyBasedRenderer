lines = None
with open("backpack.obj", "r") as f:
    lines = f.readlines()

newLines = []
for line in lines:
    newLine = line
    if line[:2] == "vt":
        yTexCoordString = line.split(" ")[2]
        yTexCoordString.split("\n")
        newYValue = 1.0 - float(yTexCoordString)
        newLine = line[:12] + str(newYValue)[:7] + line[19:]
    newLines.append(newLine)

with open("backpack.obj", "w") as f:
    f.writelines(newLines)
