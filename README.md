

#### Main CPU tự khởi động

- Bỏ `waitForAEquals1()` khỏi luồng boot.
- Main CPU không còn chờ vô hạn `CMD_CPU_TO_MASTER_START` từ màn hình
- Sau khi đọc flash, khởi tạo RTC và dò card, Main CPU tự đi vào state machine điều khiển.

#### Sửa nguy cơ treo ở vòng quét card

- Bỏ vòng `while` không giới hạn khi tìm card có IMEI/phase hợp lệ.
- Mỗi lượt chỉ kiểm tra tối đa `MAX_SIDE` card.
- Kiểm tra chỉ số trước khi truy cập `index_card[idx]`.
- Kiểm tra phase nằm trong `1..MAX_SIDE`.
- Nếu không có card hợp lệ, hàm kết thúc lượt quét và thử lại ở chu kỳ sau thay vì chạy vô hạn.

#### Bật watchdog phần cứng

- `mtfc_watdog_init()` hiện gọi `iwdg_init()` thật.
- Dùng prescaler `IWDG_PRE_256`.
- Watchdog được khởi động với timeout khoảng 3 giây.
- Watchdog chỉ bắt đầu sau giai đoạn dò card vì card detection có các khoảng chờ dài
- Main loop feed watchdog sau khi hoàn thành các tác vụ trong một vòng.


#### Kiểm tra command từ CM4

- Kiểm tra payload size trước khi xử lý các command CM4 đã biết.
- Frame thiếu dữ liệu bị bỏ qua.
- Payload struct lớn hơn hoặc bằng kích thước Main CPU yêu cầu vẫn được chấp nhận để giữ khả năng tương thích với padding struct phía CM4.
- Kiểm tra `num_side` trước khi tạo schedule và reset Main CPU.
- Lệnh START cũ vẫn hợp lệ nhưng không còn quyền quyết định Main CPU có chạy hay không.

#### Kiểm tra phase đường sắt

- Chỉ dùng `phase - 1` làm chỉ số khi phase nằm trong `1..MAX_SIDE`.
- Tránh ghi ngoài mảng nếu dữ liệu card/flash bị lỗi.


### `si-main-THGT/Main_New/lib/bsp/bsp_config.h`

- Đổi `USING_WATCHDOG_TIMER` từ `0` thành `1`.


### Khi CM4 bị tắt trong lúc đang chạy

- Main CPU không đổi state chỉ vì mất CM4.
- Chu kỳ đèn và output card tiếp tục chạy.
- Telemetry gửi sang CM4 không còn là điều kiện để state machine hoạt động.
- Frame UART bị ngắt giữa chừng sẽ được parser timeout và phục hồi.

### Khi CM4 bật lại

- CM4 vẫn gửi START theo source hiện tại; Main CPU chấp nhận nhưng không cần lệnh này để chạy.
- Các command đọc/ghi cũ tiếp tục được xử lý.

## vị trí source đã sửa

| Hạng mục | File | Dòng | Nội dung chính |
|---|---|---:|---|
| Main CPU tự khởi động | [main.cpp](./si-main-THGT/Main_New/src/main.cpp#L376-L450) | 376-450 | Bỏ chờ START, tự chạy card detect/state machine, khởi động và feed watchdog |
| Kiểm tra schedule trong flash | [main.cpp](./si-main-THGT/Main_New/src/main.cpp#L247-L263) | 247-263 | Kiểm tra `num_side`, nạp và lưu schedule mặc định nếu dữ liệu sai |
| Watchdog phần cứng | [main.cpp](./si-main-THGT/Main_New/src/main.cpp#L184-L198) | 184-198 | Tính reload và gọi `iwdg_init()` thật |
| Bật cấu hình watchdog | [bsp_config.h](./si-main-THGT/Main_New/lib/bsp/bsp_config.h#L7) | 7 | Đổi `USING_WATCHDOG_TIMER` thành `1` |

#### Đồng bộ trạng thái với Gateway

- Khi nhận `CMD_CPU_TO_MASTER_START` (`68`), Main trả ngay bốn frame:
  - `CMD 19`: chế độ nguồn/làm việc.
  - `CMD 20`: tuyến manual hiện tại; trả `0` khi không ở manual.
  - `CMD 72`: tín hiệu đường sắt sau xử lý trễ.
  - `CMD 73`: chế độ đi bộ đang thực sự hoạt động (sau khi pha xe hiện tại kết thúc).
- Main cũng gửi lại từng frame khi giá trị tương ứng thay đổi.
- Chuyển tuyến khi vẫn ở manual và thoát manual về Auto đều phát lại `CMD 20`.

### Logic đèn WG/WR

- Trong Auto, khi đèn xe xanh hoặc vàng: `WG` tắt và `WR` sáng liên tục.
- Khi đèn xe đỏ còn trên 4 giây: `WG` sáng và `WR` tắt.
- Trong 4 giây đỏ cuối (`4..1`): `WG` tắt hẳn và `WR` sáng hẳn, không chớp.
- Nhánh ánh xạ đường sắt cũng giữ đúng trạng thái `WG` tắt, `WR` sáng trong 4 giây đỏ cuối.
- Khi chế độ đi bộ do nút nhấn đang hoạt động: tất cả đèn xe về đỏ; phase được bật walking có `WG` sáng liên tục và `WR` tắt.


### Đồng bộ trạng thái với CM4/Gateway

- Bổ sung `CMD 72` để gửi trạng thái đường sắt đã qua xử lý delay.
- Bổ sung `CMD 73` để gửi trạng thái walking thực tế, không gửi trực tiếp trạng thái thô của nút.
- Khi CM4 gửi lệnh START, Main trả một snapshot gồm trạng thái làm việc, phase manual, đường sắt và walking.
- Trong khi chạy, Main chỉ gửi lại từng trạng thái khi giá trị tương ứng thay đổi.


### Vị trí chính

| Hạng mục | Vị trí |
|---|---|
 WG/WR theo 4 giây đỏ cuối | [src/main.cpp](./src/main.cpp#L3025) |
| Mã lệnh UART 72/73 | [serial_msp_code.h](./lib/bsp/serial_msp_code.h#L194) |
