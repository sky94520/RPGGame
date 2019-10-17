import os
import re


def generate_text(path, filename):
    """
    遍历path路径下的所有文件，并正则提取出所有文本，然后生成utf-8的txt文件
    :param path: 要遍历的路径名
    :param filename: 导出的文件名
    :return:
    """
    # 获取路径下的所有文件
    paths = os.listdir(path)
    # 正则匹配要显示的文本
    pattern = re.compile(r'showText\(\"(.*?)\"\s*,\s*\"(.*?)\"')
    # 用得到数字
    contents = ["1234567890"]
    # 遍历
    for name in paths:
        fp = open(os.path.join(path, name), "r", encoding="utf-8")
        lines = fp.readlines()
        text = "".join(lines)
        results = re.findall(pattern, text)
        if len(results) == 0:
            continue
        print(results)
        for result in results:
            contents.append(result[0])
            contents.append(result[1])
    # 写入文件
    with open(filename, "w", encoding="utf-8") as fp:
        fp.writelines(contents)
        print("写入文件成功:%s" % filename)


def generate_fnt(bmfc, input, path, filename):
    """
    根据配置文件和输入的文本文件来导出对应的fnt文件和png文件
    :param bmfc: BMFont的配置文件
    :param input: 文本文件
    :param path: 输出路径
    :param filename: 输出文件名称
    :return:
    """
    # 判断输出文件夹是否存在
    if not os.path.exists(path):
        os.mkdir(path)
        print("创建文件夹成功", path)
    ret = os.system("bmfont -c %s -t %s -o %s" % (bmfc, input, os.path.join(path, filename)))
    # 导出成功
    if ret == 0:
        print("fnt文件导出成功")


if __name__ == '__main__':
    # 主目录
    base_path = os.path.join(".", "RPGGame", "Resources")
    # 先提取出要用到的文本,并生成文件
    generate_text(os.path.join(base_path, "script"), "1.txt")
    # 使用bmfont 命令行导出资源文件，并放到对应的文件夹中
    generate_fnt("1.bmfc", "1.txt", os.path.join(base_path, "fonts"), "1.fnt")
