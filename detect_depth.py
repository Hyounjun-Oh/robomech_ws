import argparse
import time

import cv2
import numpy as np
import pyrealsense2 as rs
import torch
import torch.backends.cudnn as cudnn
from numpy import random

from models.experimental import attempt_load
from utils.augmentations import letterbox
from utils.general import check_img_size, non_max_suppression, scale_coords, set_logging
from utils.plots import plot_one_box
from utils.torch_utils import select_device, time_sync

DEPTH_STREAM = (1280, 720, rs.format.z16, 30)
COLOR_STREAM = (1920, 1080, rs.format.bgr8, 30)
DEPTH_SAMPLES = 40  # 박스 중심 주변에서 샘플링할 깊이 값 개수


def start_realsense():
    """깊이/컬러 스트림을 시작하고 (pipeline, 깊이->컬러 정렬기)를 반환한다."""
    config = rs.config()
    config.enable_stream(rs.stream.depth, *DEPTH_STREAM)
    config.enable_stream(rs.stream.color, *COLOR_STREAM)
    pipeline = rs.pipeline()
    pipeline.start(config)
    return pipeline, rs.align(rs.stream.color)


def preprocess(image, imgsz, half, device):
    """BGR HWC uint8 이미지 -> 모델 입력 텐서 (1x3xHxW, 0~1)"""
    img = letterbox(image, new_shape=imgsz)[0]
    img = img[:, :, ::-1].transpose(2, 0, 1)  # BGR to RGB, HWC to CHW
    img = np.ascontiguousarray(img, dtype=np.float16 if half else np.float32)
    img /= 255.0  # 0 - 255 to 0.0 - 1.0
    return torch.from_numpy(img).to(device).unsqueeze(0)


def estimate_distance(depth_frame, xyxy, samples=DEPTH_SAMPLES):
    """박스 중심 주변을 무작위로 샘플링해, 중간 50% 깊이 값의 평균 [m]을 반환한다."""
    x1, y1, x2, y2 = (int(v) for v in xyxy)
    cx, cy = (x1 + x2) // 2, (y1 + y2) // 2
    spread = max(min(abs(x2 - x1), abs(y2 - y1)) // 4, 1)

    distances = []
    for _ in range(samples):
        bias = random.randint(-spread, spread)
        dist = depth_frame.get_distance(cx + bias, cy + bias)
        if dist:
            distances.append(dist)

    distances = np.sort(distances)[samples // 2 - samples // 4:samples // 2 + samples // 4]
    return np.mean(distances)


def detect(opt):
    set_logging()
    device = select_device(opt.device)
    half = device.type != 'cpu'  # half precision only supported on CUDA

    # Load model
    model = attempt_load(opt.weights, map_location=device)  # load FP32 model
    imgsz = check_img_size(opt.img_size, s=model.stride.max())  # check img_size
    if half:
        model.half()  # to FP16
    cudnn.benchmark = True  # set True to speed up constant image size inference

    # Get names and colors
    names = model.module.names if hasattr(model, 'module') else model.names
    colors = [[random.randint(0, 255) for _ in range(3)] for _ in range(len(names))]

    # Run inference once to warm up
    t0 = time.time()
    if half:
        model(torch.zeros((1, 3, imgsz, imgsz), device=device).half())

    pipeline, align_to_color = start_realsense()
    try:
        while True:
            # Wait for a coherent pair of frames: depth and color
            frames = align_to_color.process(pipeline.wait_for_frames())
            depth_frame = frames.get_depth_frame()
            im0 = np.asanyarray(frames.get_color_frame().get_data()).copy()

            img = preprocess(im0, imgsz, half, device)
            t1 = time_sync()
            pred = model(img, augment=opt.augment)[0]
            det = non_max_suppression(pred, opt.conf_thres, opt.iou_thres,
                                      classes=opt.classes, agnostic=opt.agnostic_nms)[0]
            t2 = time_sync()

            s = '0: %gx%g ' % img.shape[2:]
            if det is not None and len(det):
                # Rescale boxes from img_size to im0 size
                det[:, :4] = scale_coords(img.shape[2:], det[:, :4], im0.shape).round()

                for c in det[:, -1].unique():
                    n = (det[:, -1] == c).sum()  # detections per class
                    s += '%g %ss, ' % (n, names[int(c)])

                for *xyxy, conf, cls in reversed(det):
                    label = '%s %.2fm' % (names[int(cls)], estimate_distance(depth_frame, xyxy))
                    plot_one_box(xyxy, im0, label=label, color=colors[int(cls)], line_thickness=3)

            # Print time (inference + NMS)
            print('%sDone. (%.3fs)' % (s, t2 - t1))

            cv2.imshow(opt.source, im0)
            if cv2.waitKey(1) == ord('q'):  # q to quit
                break
    finally:
        pipeline.stop()
        cv2.destroyAllWindows()
    print('Done. (%.3fs)' % (time.time() - t0))


def parse_opt():
    parser = argparse.ArgumentParser()
    parser.add_argument('--weights', nargs='+', type=str, default='yolov5m.pt', help='model.pt path(s)')
    parser.add_argument('--source', type=str, default='inference/images', help='display window name')
    parser.add_argument('--img-size', type=int, default=640, help='inference size (pixels)')
    parser.add_argument('--conf-thres', type=float, default=0.25, help='object confidence threshold')
    parser.add_argument('--iou-thres', type=float, default=0.45, help='IOU threshold for NMS')
    parser.add_argument('--device', default='', help='cuda device, i.e. 0 or 0,1,2,3 or cpu')
    parser.add_argument('--classes', nargs='+', type=int, help='filter by class: --class 0, or --class 0 2 3')
    parser.add_argument('--agnostic-nms', action='store_true', help='class-agnostic NMS')
    parser.add_argument('--augment', action='store_true', help='augmented inference')
    return parser.parse_args()


if __name__ == '__main__':
    opt = parse_opt()
    print(opt)

    with torch.no_grad():
        detect(opt)
