// 

// Time-align three data series.

const double SYNC_INTERVAL = 44100 * 5;

const double REFINE_STEP = 0.333;
const double REFINE_WIDTH = 44100 * 0.1;

const double FINE_REFINE_STEP = 0.05;
const double FINE_REFINE_WIDTH = 3.0;

struct Sync {
  Sync() {
    index[0] = index[1] = index[2] = 0;
  }

  double index[3];
};

void FindFullAlignment(const vector<float>& source[3],
                       vector<Sync>* sync_points) {
  Sync sync;
  FindInitialAlignment(source, &sync);
  sync_points->push_back(sync);

  while (AdvanceSync(source, &sync)) {
    // keep going
  }
}


void FindInitialAlignment(const vector<float>& source[3],
                          Sync* sync) {
  sync.index[0] = ...;
  sync.index[1] = FindBestAlignment(
      source[0], source[1], ..., ..., ...);
  ...;
}

bool AdvanceSync(const vector<float>& source[3],
                 Sync* sync) {
  sync->index[0] += SYNC_INTERVAL;
  sync->index[1] += SYNC_INTERVAL;
  sync->index[2] += SYNC_INTERVAL;

  if ((sync->index[0] + REFINE_WIDTH) << 1 >= source[0].size() ||
      (sync->index[1] + REFINE_WIDTH) << 1 >= source[1].size() ||
      (sync->index[2] + REFINE_WIDTH) << 1 >= source[2].size()) {
    return false;
  }

  sync->index[1] = RefineSync(source[0], source[1],
                              sync->index[0], sync->index[1]);
  sync->index[2] = RefineSync(source[1], source[2],
                              sync->index[1], sync->index[2]);
  return true;
}

void RefineSync(const vector<float>& s0, const vector<float>& s1,
                double i0, double i1) {
  double best_align = FindBestAlignment(
      s0, s1, i0, i1, REFINE_WIDTH, REFINE_WIDTH, REFINE_STEP);

  // Refine again with very small deltas.
  best_align = FindBestAlignment(
      s0, s1, i0, best_align, FINE_REFINE_WIDTH, REFINE_WIDTH,
      FINE_REFINE_STEP);

  return best_align;
}

double FindBestAlignment(const vetor<float>& s0, const vector<float>& s1,
                         double i0, double i1,
                         double search_width, double width, double step) {
  double best_err = 1e20;
  double best_delta = 0;
  for (double delta = i1 - search_width / 2;
       delta < best_delta + search_width / 2;
       delta += step) {
    double err = RmsDiff(s0, s1, i0, i1 + delta, width);
    if (err < best_err) {
      best_err = err;
      best_delta = delta;
    }
  }

  return i1 + best_delta;
}

double RmsDiff(const vector<float>& s0, const vector<float>& s1,
               double i0, double i1, double width) {
  double diff2 = 0;
  for (double d = -width / 2; d < width / 2; d += 1) {
    pair<double, double> x0 = Sample(s0, i0 + d);
    pair<double, double> x1 = Sample(s1, i1 + d);
    double diffl = x0.first - x1.first;
    double diffr = x0.second - x1.second;
    diff2 += (diffl * diffl) + (diffr * diffr);
  }
  return sqrt(diff2);
}

pair<double, double> Sample(const vector<float>& data, double offset) {
  pair<double, double> result;

  // Cubic interpolation.
  size_t index = size_t(floor(offset));
  double fraction = offset - index;
  double x[4][2] = {
    { data[index - 2], data[index - 1] },
    { data[index], data[index + 1] },
    { data[index + 2], data[index + 3] },
    { data[index + 4], data[index + 5] }
  };
  // Left.
  {
    double a = (3 * (x[1][0] - x[2][0]) - x[0][0] + x[3][0]) / 2;
    double b = 2 * x[2][0] + x[0][0] - (5 * x[1][0] + x[3][0]) / 2;
    double c = (x[2][0] - x[0][0]) / 2;
    result.first = (((a * fraction) + b) * fraction + c) * fraction + x0;
  }
  // Right.
  {
    double a = (3 * (x[1][1] - x[2][1]) - x[0][1] + x[3][1]) / 2;
    double b = 2 * x[2][1] + x[0][1] - (5 * x[1][1] + x[3][1]) / 2;
    double c = (x[2][1] - x[0][1]) / 2;
    result.second = (((a * fraction) + b) * fraction + c) * fraction + x0;
  }
  return result;
}
