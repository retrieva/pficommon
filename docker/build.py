import datetime
import os
import subprocess


IMAGE_BASENAME = 'docker.retrieva.jp/pficommon_ci'
base_images_and_tags = {
    'gcc': [
        '5',
        '6',
        '7',
        '8',
        '9',
    ]
}


def build_image(base_image, tag, build_date):
    image_name = '{}:{}{}.{}'.format(
        IMAGE_BASENAME, base_image, tag, build_date)
    dockerfile_path = os.path.join(base_image, 'Dockerfile')
    cmd = "docker build --no-cache --build-arg image_tag={} " \
          "-t {} -f {} ..".format(tag, image_name, dockerfile_path)
    subprocess.run(cmd, shell=True)


def build_images(base_images_and_tags, build_date):
    for base_image, tags in base_images_and_tags.items():
        for tag in tags:
            build_image(base_image, tag, build_date)


if __name__ == '__main__':
    today = datetime.date.today().strftime('%y%m')
    build_images(base_images_and_tags, today)
