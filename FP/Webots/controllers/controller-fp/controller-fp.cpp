// Template FP-1
// Referensi model robot: https://webots.cloud/run?version=R2025a&url=https%3A%2F%2Fgithub.com%2Fcyberbotics%2Fwebots%2Fblob%2Freleased%2Fprojects%2Frobots%2Frobotis%2Fdarwin-op%2Fprotos%2FRobotisOp2.proto
// Referensi Api Function: https://cyberbotics.com/doc/reference/nodes-and-api-functions

#include <webots/Robot.hpp>
#include <webots/Motor.hpp>
#include <webots/PositionSensor.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <string>
#include <iomanip>
#include <algorithm>

#define NLOHMANN_JSON_NO_EXCEPTIONS 1
#include "json.hpp"


using namespace webots;
using namespace std;
using json = nlohmann::json;

enum class Mode { Single, Pair };
enum class Action { Increment, Decrement };

Robot *robot;
int timestep;
double INCREMENT = 0.1;
int selection = 0;
Mode current_mode = Mode::Single;

vector<string> motor_names = {
  "Head","Neck","ShoulderL","ShoulderR","ArmUpperL","ArmUpperR",
  "ArmLowerL","ArmLowerR","PelvYL","PelvYR","PelvL","PelvR",
  "LegUpperL","LegUpperR","LegLowerL","LegLowerR","AnkleL","AnkleR",
  "FootL","FootR"
};

map<string, Motor*> motors;
map<string, PositionSensor*> pos_sensors;


ifstream espSerial("COM3"); 
map<string,int> keys_serial; 
double esp_inc = 0.1;

vector<string> esp_keys = {"W","A","S","D","Q","E","C","V","R","F", "Z",};

string pose_folder = "../../poses/";
vector<string> pose_files = {
  "pose-jongkok",
  "pose-berdiri",
  "pose-jalan-maju",
  "pose-jalan-mundur",
  "pose-geser-kanan",
  "pose-geser-kiri",
  "pose-belok-kanan",
  "pose-belok-kiri"
};

using Pose = pair<string, vector<double>>; 
map<string, vector<Pose>> pose_data;       
int pose_group_index = 0;
int pose_index = 0;
vector<string> pose_group_names; 

void print_pose_menu() {
  cout << "\n[Pose Play] Groups:";
  if (pose_group_names.empty()) { cout << " (none)\n"; return; }
  cout << "  Selected Group: " << pose_group_names[pose_group_index] << "\n";
  const auto &gname = pose_group_names[pose_group_index];
  if (!pose_data.count(gname) || pose_data[gname].empty()) {
    cout << "  Poses in '" << gname << "': (none)\n";
    return;
  }
  cout << "  Poses in '" << gname << "': ";
  const auto &plist = pose_data[gname]; 
  for (int i = 0; i < (int)plist.size(); ++i) {
    const auto &pname = plist[i].first;
    if (i == pose_index) cout << "[" << pname << "] ";
    else cout << pname << " ";
  }
  cout << "\n";
}

void run_pose_file(const string &filename) {
  string fpath = pose_folder + filename;
  ifstream in(fpath);
  if (!in.is_open()) {
    cout << "[ERROR] Tidak bisa buka file: " << fpath << endl;
    return;
  }

  json j;
  in >> j;

  if (!j.is_object() || !j.contains("pose_group")) {
    cout << "[ERROR] Format pose tidak valid: " << fpath << endl;
    return;
  }

  for (const auto &grp : j["pose_group"]) {
    if (!grp.contains("pose")) continue;
    for (const auto &p : grp["pose"]) {
      if (!p.contains("posisi")) continue;
      vector<double> posisi;
      for (const auto &v : p["posisi"])
        posisi.push_back(v.get<double>());
      write_motion(posisi, 10);
    }
  }

  cout << "[OK] Pose dari " << filename << " dieksekusi.\n";
}


void write_motion(const vector<double> &target_pos_list, int steps = 10) {
  if ((int)target_pos_list.size() != (int)motor_names.size()) {
    cout << "[ERROR] Position list length mismatch: expected " << motor_names.size()
         << ", got " << target_pos_list.size() << "\n";
    return;
  }
  vector<double> start_positions(motor_names.size(), 0.0);
  for (size_t i = 0; i < motor_names.size(); ++i) {
    const auto &name = motor_names[i];
    if (pos_sensors.count(name)) start_positions[i] = pos_sensors[name]->getValue();
  }
  vector<double> deltas(motor_names.size(), 0.0);
  for (size_t i = 0; i < motor_names.size(); ++i)
    deltas[i] = target_pos_list[i] - start_positions[i];

  int s = max(1, steps);
  for (int step = 1; step <= s; ++step) {
    double frac = (double)step / (double)s;
    for (size_t i = 0; i < motor_names.size(); ++i) {
      const auto &name = motor_names[i];
      double new_pos = start_positions[i] + deltas[i] * frac;
      if (motors.count(name)) motors[name]->setPosition(new_pos);
    }
    if (robot->step(timestep) == -1) return;
  }
}

void execute_selected_pose() {
  if (pose_group_names.empty()) return;
  const auto &gname = pose_group_names[pose_group_index];
  if (!pose_data.count(gname) || pose_data[gname].empty()) return;

  const auto &plist = pose_data[gname];
  int maxPose = (int)plist.size() - 1;
  int idx = std::max(0, std::min(pose_index, maxPose));

  const string &selected_pose_name = plist[idx].first;
  cout << "Executing Pose: " << selected_pose_name << " from Group: " << gname << "\n";
  write_motion(plist[idx].second, 10);
}


// === FUNCTIONS ===
void setup_motor() {
  cout << "Initializing motors and sensors..." << endl;
  for (auto &name : motor_names) {
    Motor *motor = robot->getMotor(name);
    if (!motor) {
      cerr << "[WARN] Motor not found: " << name << endl;
      continue;
    }
    motors[name] = motor;

    string sname = name + "S";
    PositionSensor *sensor = robot->getPositionSensor(sname);
    if (sensor) {
      sensor->enable(timestep);
      pos_sensors[name] = sensor;
      cout << "[OK] Sensor enabled: " << sname << endl;
      motor->setPosition(sensor->getValue());
    } else {
      motor->setPosition(0.0);
    }
  }
}

void get_all_pos() {
  cout << "[Positions] ";
  for (auto &name : motor_names) {
    if (pos_sensors.count(name))
      cout << fixed << setprecision(3) << pos_sensors[name]->getValue() << " ";
    else
      cout << "N/A ";
  }
  cout << endl;
}

void print_joint_list() {
  cout << "INCREMENT: " << fixed << setprecision(3) << INCREMENT << endl;
  cout << "Current mode: " << (current_mode == Mode::Single ? "Single" : "Pair") << endl;
  if (current_mode == Mode::Single)
    cout << "Selection: " << selection << endl;
  else
    cout << "Selection: " << selection << " and " << selection+1 << endl;

  cout << "--- Joint list ---" << endl;
  for (size_t i=0;i<motor_names.size();++i) {
    if (current_mode==Mode::Pair && (int)i==selection+1) continue;
    cout << "[" << setw(2) << i << "] " << motor_names[i] << ": ";
    if (pos_sensors.count(motor_names[i]))
      cout << fixed << setprecision(3) << pos_sensors[motor_names[i]]->getValue();
    else
      cout << "N/A";
    if ((int)i==selection) cout << " <== SELECTED";
    cout << endl;
  }
  get_all_pos();
}

void adjust_motor(Mode mode, Action act) {
  if (mode==Mode::Single) {
    string name = motor_names[selection];
    double val = pos_sensors[name]->getValue();
    motors[name]->setPosition(act==Action::Increment ? val+INCREMENT : val-INCREMENT);
  } else {
    string n1 = motor_names[selection];
    string n2 = motor_names[selection+1];
    double v1 = pos_sensors[n1]->getValue();
    double v2 = pos_sensors[n2]->getValue();
    if (act==Action::Increment) {
      motors[n1]->setPosition(v1+INCREMENT);
      motors[n2]->setPosition(v2-INCREMENT);
    } else {
      motors[n1]->setPosition(v1-INCREMENT);
      motors[n2]->setPosition(v2+INCREMENT);
    }
  }
}

void zero_motor() {
  if (current_mode==Mode::Single)
    motors[motor_names[selection]]->setPosition(0.0);
  else {
    motors[motor_names[selection]]->setPosition(0.0);
    motors[motor_names[selection+1]]->setPosition(0.0);
  }
}


bool readESP() {
  if (!espSerial.is_open()) return false;
  string line;
  if (!getline(espSerial,line)) return false;

  stringstream ss(line);
  string item;
  int idx = 0;
  while (getline(ss,item,',')) {
    if (idx < 10) {
      keys_serial[esp_keys[idx]] = stoi(item);
    } else {
      esp_inc = stof(item);
    }
    idx++;
  }
  return true;
}

int main() {
  robot = new Robot();
  timestep = (int)robot->getBasicTimeStep();

  setup_motor();
  cout << "ESP Pose Controller started. Waiting for serial input...\n";

  // init semua key ke 0
  for (auto &k : esp_keys) keys_serial[k] = 0;

  while (robot->step(timestep) != -1) {
    print_joint_list();

    if (readESP()) {
      INCREMENT = esp_inc;
      if (keys_serial["C"]) run_pose_file("pose-belok-kanan.json");
      if (keys_serial["Z"]) run_pose_file("pose-belok-kiri.json");
      if (keys_serial["W"]) run_pose_file("pose-jalan-maju.json");
      if (keys_serial["S"]) run_pose_file("pose-jalan-mundur.json");
      if (keys_serial["D"]) run_pose_file("pose-geser-kanan.json");
      if (keys_serial["A"]) run_pose_file("pose-geser-kiri.json");
      if (keys_serial["Q"]) run_pose_file("pose-berdiri.json");
      if (keys_serial["E"]) run_pose_file("pose-jongkok.json");
    }
  }

  return 0;
}
