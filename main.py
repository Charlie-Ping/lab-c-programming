import cv2
import numpy as np


# HSV颜色范围

lower_red1 = np.array([0, 150, 150])     # 低亮度红色
upper_red1 = np.array([10, 255, 255])
lower_red2 = np.array([170, 150, 150])   # 高亮度红色
upper_red2 = np.array([180, 255, 255])
# 打开摄像头
cap = cv2.VideoCapture(0)

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # 将图像转换为HSV色彩空间
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    # 创建遮罩，提取红色区域
    mask1 = cv2.inRange(hsv, lower_red1, upper_red1)
    mask2 = cv2.inRange(hsv, lower_red2, upper_red2)
    mask = mask1 | mask2

    # 去除噪点
    # mask = cv2.morphologyEx(mask, cv2.MORPH_OPEN, np.ones((5, 5), np.uint8))
    # mask = cv2.morphologyEx(mask, cv2.MORPH_DILATE, np.ones((5, 5), np.uint8))

    # 查找轮廓
    contours, _ = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    # 遍历轮廓，查找红色方块
    for contour in contours:
        # 计算轮廓的边界框
        x, y, w, h = cv2.boundingRect(contour)
        # 类似的函数有fitEllipse() 识别椭圆, 
        
        # 根据比例判断是否是长方体（长宽比范围可以调整）
        aspect_ratio = w / float(h)
        if (0.5 <= aspect_ratio <= 2.0) and (w > 20 and h > 20):  # 过滤小的噪声
            # 绘制红色长方体边界
            cv2.rectangle(frame, (x, y), (x + w, y + h), (0, 255, 0), 2)
            cv2.putText(frame, "Red Rectangle", (x, y - 10), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # 显示结果
    cv2.imshow("Red Block Detection", frame)
    cv2.imshow("Mask", mask)
    
    # 按下 'q' 键退出
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# 释放摄像头和窗口
cap.release()
cv2.destroyAllWindows()
