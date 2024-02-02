from pathlib import Path
from datetime import datetime
from typing import List


EXTENSIONS = ['.cpp', '.cc', '.cxx', '.c', '.hpp', '.h']


current_dir = Path(__file__).parent
current_year = str(datetime.utcnow().year)


def format_buffer(s: str) -> str:
    return s.replace('%year%', current_year)


with open(current_dir / 'license_template.txt', 'r') as f:
    license_template = format_buffer(f.read())

with open(current_dir / 'license_template_header.txt', 'r') as f:
    license_template_header = format_buffer(f.read())


def get_comment_chr(file_name: str) -> str:
    return '#' if file_name.endswith('.py') else '//'


def to_comments(file_name: str, buffer: str) -> str:
    comment_chr: str = get_comment_chr(file_name)
    return '\n'.join([f'{comment_chr} {line}'.strip() for line in buffer.splitlines()])


def remove_copyright(file_name: str, lines: List[str]) -> List[str]:
    comment_chr: str = get_comment_chr(file_name)

    ends_at: int = 0
    for i, line in enumerate(lines):
        if line.startswith(comment_chr):
            continue

        ends_at = max(0, i)
        break

    starts_at_bottom: int = len(lines) - 1
    for i, line in reversed(list(enumerate(lines))):
        if line.startswith(comment_chr) or line == '':
            continue

        starts_at_bottom = min(len(lines) - 1, i + 1)
        break

    return lines[ends_at:starts_at_bottom]


def append_copyright_to(p: Path) -> None:
    header = to_comments(p.name, license_template_header).splitlines()
    full = to_comments(p.name, license_template).splitlines()

    with open(p, 'r') as f:
        file_lines = f.read().splitlines()

    def _is_copyrighted() -> bool:
        index = header[0].find(current_year)
        assert index != -1
        return file_lines[0][:index] == header[0][:index]
    
    is_copyrighted = _is_copyrighted()
    if is_copyrighted:
        file_lines = remove_copyright(p.name, file_lines)

    file_lines = header + file_lines + [''] + full + ['']
    with open(p, 'w') as f:
        f.write('\n'.join(file_lines))


def iter_dir(p: Path) -> None:
    for file in p.iterdir():
        if file.is_dir():
            iter_dir(file)
            continue

        for ext in EXTENSIONS:
            if not file.name.endswith(ext):
                continue

            append_copyright_to(file)
            break


if __name__ == '__main__':
    iter_dir(Path(__file__).parent.parent)

