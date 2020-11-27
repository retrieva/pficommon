import datetime
import os
import subprocess


IMAGE_BASENAME = 'docker.retrieva.jp/pficommon_ci'
base_images_and_tags = {
    'gcc': [
        '4.9',
        '5',
        '6',
        '7',
        '8',
        '9',
    ],
    'centos': [
        '7',
        '8',
    ]
}


def get_docker_build_command(image_name, base_image, tag):
    dockerfile_path_per_tag = os.path.join(base_image, tag, 'Dockerfile')
    if os.path.exists(dockerfile_path_per_tag):
        return "docker build --no-cache -t {} -f {} ..".format(
            image_name, dockerfile_path_per_tag)
    else:
        dockerfile_path = os.path.join(base_image, 'Dockerfile')
        return "docker build --no-cache --build-arg image_tag={} " \
               "-t {} -f {} ..".format(tag, image_name, dockerfile_path)


def build_image(base_image, tag, build_date):
    image_name = '{}:{}{}.{}'.format(
        IMAGE_BASENAME, base_image, tag, build_date)
    cmd = get_docker_build_command(image_name, base_image, tag)
    subprocess.run(cmd, shell=True)


def build_images(base_images_and_tags, build_date):
    for base_image, tags in base_images_and_tags.items():
        for tag in tags:
            build_image(base_image, tag, build_date)


if __name__ == '__main__':
    today = datetime.date.today().strftime('%y%m')
    build_images(base_images_and_tags, today)
