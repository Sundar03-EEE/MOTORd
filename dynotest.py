import serial
import time
import matplotlib.pyplot as plt

PICO_PORT = 'COM4'
MKS_PORT = 'COM5'
BAUDRATE = 115200
brake_min = 5000
brake_step = 15  # This was changed

 

def open_serial(port):
    return serial.Serial(port, BAUDRATE, timeout=5)


def send(ser, msg):
    ser.write(f"{msg}\n".encode())
    print(f"[→] {msg}")
    time.sleep(0.01)


def wait_for(ser, keyword, timeout=6):
    start = time.time()
    while time.time() - start < timeout:
        line = ser.readline().decode(errors='ignore').strip()
        if keyword in line:
            print(f"[✓] {line}")
            return True
    print(f"[!] Timeout waiting for: {keyword}")
    return False


def monitor_until_failure(pico, set_rpm, timeout=1): # This was changed
    threshold = set_rpm * 0.95  # Failure will be detected if RPM drops below 5% of the set rpm
    current_rpm = set_rpm
    last_torque = 0.0
    start = time.time()

    while time.time() - start < timeout:
        rpm_val, torque_val = None, None

        while pico.in_waiting:
            line = pico.readline().decode(errors='ignore').strip()
            if line.startswith("RPM:"):
                try:
                    rpm_val = float(line.split(":")[1])
                except: continue
            elif line.startswith("Torque:"):
                try:
                    torque_val = float(line.split(":")[1])
                except: continue

            # Only print/log when both are fresh
            if rpm_val is not None and torque_val is not None:
                current_rpm = rpm_val
                last_torque = torque_val
                
                print(f"    RPM: {current_rpm:.2f}, Torque: {last_torque:.2f}")
                if current_rpm < threshold:
                    print(f"[✓] FAILURE Detected → RPM dropped. RPM: {current_rpm:.2f}, Torque: {last_torque:.2f}")
                    return last_torque
                break

        plt.pause(0.002)

    print("[!] Failure not detected in time.")
    return 0.0

# Changed the tolerance from 10 to 15
def wait_until_stable_rpm(pico, target_rpm, tolerance=15, timeout=20):
    start = time.time()
    current_rpm = 0.0
    while time.time() - start < timeout:
        if pico.in_waiting:
            line = pico.readline().decode(errors='ignore').strip()
            if line.startswith("RPM:"):
                try:
                    current_rpm = float(line.split(":")[1])
                    print(f"  [INFO] Waiting RPM settle: {current_rpm:.2f}")
                    if abs(current_rpm - target_rpm) < (target_rpm * tolerance / 100):
                        return True
                except: continue
        plt.pause(0.002)
    return False


def record_live_rpm(pico, duration=5): # Changed the duration from 2 to 5 seconds
    """Record live RPM 5 seconds after stabilization."""
    start = time.time()
    live_rpm = None
    while time.time() - start < duration:
        if pico.in_waiting:
            line = pico.readline().decode(errors='ignore').strip()
            if line.startswith("RPM:"):
                try:
                    live_rpm = float(line.split(":")[1])
                    print(f"  [LIVE] Recorded RPM: {live_rpm:.2f}")
                    return live_rpm
                except:
                    continue
    print("[!] Failed to record live RPM.")
    return None


def run_test(start_rpm, end_rpm, step_rpm, iteration, color):
    pico = open_serial(PICO_PORT)
    mks = open_serial(MKS_PORT)
    time.sleep(1)
    send(mks, "OY")
    send(mks, "OX")
    send(mks, "H")
    wait_for(mks, "Y-axis Homed to 5000", timeout=12)

    rpm_range = list(range(start_rpm, end_rpm + 1, step_rpm))
    rpms = []  # Initialize rpms as an empty list
    torques = []  # Initialize torques as an empty list

    for rpm in rpm_range:
        print(f"\n🌀 Testing RPM: {rpm} (Iteration {iteration})")
        send(pico, "tare")
        if not wait_for(pico, "Tare complete.", timeout=6):
            continue

        send(pico, "RESET")
        send(pico, "brake:5000")
        send(mks, str(rpm))
        if not wait_until_stable_rpm(pico, rpm, timeout=20):
            print("[!] RPM did not settle. Skipping.")
            continue

        print("  [INFO] Waiting 5 seconds after RPM stabilizes...")
        time.sleep(5)
        rec_rpm = record_live_rpm(pico, duration=5)
        if rec_rpm is None:
            print("[!] Failed to record live RPM. Skipping.")
            continue

        brake_pos = brake_min
        while brake_pos <= 10000:
            print(f"  🛑 Brake: {brake_pos}")
            send(mks, f"Y{brake_pos}")
            send(pico, f"brake:{brake_pos}")

            torque = monitor_until_failure(pico, rec_rpm, timeout=1)

            if torque > 0:
                # Plot the live torque value at failure with rec_rpm
                rpms.append(rec_rpm)
                torques.append(torque)
                plt.plot(rpms, torques, f'{color}o-', label=f"Iteration {iteration}" if len(rpms) == 1 else "")
                plt.pause(0.01)
                send(mks, "H")
                wait_for(mks, "Y-axis Homed to 5000", timeout=12)
                break

            brake_pos += brake_step

    """ Reset the brake to 0 after the test is done"""

    send(mks, "B")
    wait_for(mks, "Y-axis Reset", timeout=12)

    pico.close()
    mks.close()
    return rpms, torques

if __name__ == "__main__":
    plt.ion()
    fig, ax = plt.subplots()
    ax.set_xlabel("RPM")
    ax.set_ylabel("Peak Torque (Nm)")
    ax.set_title("Torque vs RPM (Multiple Iterations)")
    ax.grid(True)

    colors = ['r', 'g', 'b', 'c', 'm', 'y']  # Colors for the three iterations
    all_rpms, all_torques = [], []
    start_rpm = int(input("Start RPM: "))
    end_rpm = int(input("End RPM: "))
    step_rpm = int(input("Step RPM: "))

    for i in range(2):  # Run the test 3 times
        

        rpms, torques = run_test(
            start_rpm,
            end_rpm,
            step_rpm,
            iteration=i + 1,
            color=colors[i]
        )
        all_rpms.append(rpms)
        all_torques.append(torques)



    plt.legend()
    plt.ioff()
    plt.savefig("torque_vs_rpm_iterations.png")
    plt.show()

    # Save results to CSV
    with open("torque_vs_rpm_iterations.csv", "w") as f:
        f.write("Iteration,RPM,Torque(Nm)\n")
        for i, (rpms, torques) in enumerate(zip(all_rpms, all_torques), start=1):
            for r, t in zip(rpms, torques):
                f.write(f"{i},{r},{t:.2f}\n")

    print("[✓] Test complete. Results saved.")















