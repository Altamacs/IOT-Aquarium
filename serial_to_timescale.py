import serial
import time
import psycopg2
from datetime import datetime
import sys

# Serial port configuration
SERIAL_PORT = 'COM4'  # Change this to match your port
BAUD_RATE = 115200

# Database configuration
DB_NAME = "aquarium"
DB_USER = "aquarium"
DB_PASSWORD = "aquarium123"
DB_HOST = "localhost"
DB_PORT = "5432"

def connect_to_db():
    try:
        conn = psycopg2.connect(
            dbname=DB_NAME,
            user=DB_USER,
            password=DB_PASSWORD,
            host=DB_HOST,
            port=DB_PORT
        )
        return conn
    except Exception as e:
        print(f"Error connecting to database: {e}")
        sys.exit(1)

def insert_data(conn, timestamp, food_level, light_value):
    try:
        cur = conn.cursor()
        cur.execute(
            "INSERT INTO food_levels (time, food_level, light_value) VALUES (%s, %s, %s)",
            (timestamp, food_level, light_value)
        )
        conn.commit()
    except Exception as e:
        print(f"Error inserting data: {e}")
        conn.rollback()

def main():
    # Connect to database
    conn = connect_to_db()
    print("Connected to database")

    # Open serial port
    try:
        ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
        print(f"Connected to {SERIAL_PORT}")
    except Exception as e:
        print(f"Error opening serial port: {e}")
        sys.exit(1)

    try:
        while True:
            if ser.in_waiting:
                # Read line from serial, ignore decode errors
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                
                # Skip debug messages and empty lines
                if not line or line.startswith("Light:") or line.startswith("Error"):
                    continue
                
                # Only process lines that look like CSV
                if line.count(',') != 2:
                    continue

                try:
                    # Parse CSV data
                    timestamp, food_level, light_value = line.split(',')
                    
                    # Convert to appropriate types
                    timestamp = datetime.fromtimestamp(int(timestamp) / 1000.0)  # Convert milliseconds to datetime
                    food_level = float(food_level)
                    light_value = int(light_value)
                    
                    # Insert into database
                    insert_data(conn, timestamp, food_level, light_value)
                    print(f"Inserted: {timestamp}, Food: {food_level}%, Light: {light_value}")
                    
                except ValueError as e:
                    print(f"Error parsing data: {e}")
                    continue
                
            time.sleep(0.1)  # Small delay to prevent CPU overuse
            
    except KeyboardInterrupt:
        print("\nStopping...")
    finally:
        ser.close()
        conn.close()
        print("Disconnected")

if __name__ == "__main__":
    main() 