#pragma once

#include <vector>
#include <iostream>
#include <algorithm>

class Point
{
public:
	Point() {}
	Point(int x, int y) :x_(x), y_(y) {}

	int x_, y_;
};

class Edge
{
public:
	Edge(Point p1,Point p2)
	{
		y_max_ = p1.y_; y_min_ = p1.y_; x_of_y_min_ = p1.x_;
		if (y_max_ < p2.y_)
		{
			y_max_ = p2.y_;
		}
		else if(y_max_>p2.y_)
		{
			y_min_ = p2.y_;
			x_of_y_min_ = p2.x_;
		}
		slash_slope_ = (p1.x_ - p2.x_) / float(p1.y_ - p2.y_);
	}

	int y_max_, y_min_;
	float slash_slope_;
	float x_of_y_min_;
};

class ScanLine
{
public:
	ScanLine();
	~ScanLine();

	void ConstructEdgeTable(const std::vector<int> &vx, const std::vector<int> &vy)
	{
		if (vx.size() != vy.size() || vx.size() <= 1)
		{
			std::cout << "EDGE DATA ERROR!" << std::endl;
		}

		for (int i = 0; i < vx.size(); i++)
		{
			y_max_ = std::max(y_max_, vy[i]);
			if (vy[i] != vy[(i + 1) % vy.size()])
			{
				edge_table_.push_back(Edge(Point(vx[i], vy[i]), Point(vx[(i + 1) % vy.size()], vy[(i + 1) % vy.size()])));
			}
		}

		std::sort(edge_table_.begin(), edge_table_.end(), [](Edge e1, Edge e2) {return e1.y_min_ < e2.y_min_; });
	}

	void ConstructAEL(int y)
	{
		while (edge_table_.size() > 0 && edge_table_[0].y_min_ == y)
		{
			active_edge_list_.push_back(edge_table_[0]);
			edge_table_.erase(edge_table_.begin());
		}
	}

	void UpdateAEL(int y)
	{
		for (int i = 0; i < active_edge_list_.size(); )
		{
			if (active_edge_list_[i].y_max_ <= y)
			{
				active_edge_list_.erase(active_edge_list_.begin() + i);
			}
			else
			{
				if (active_edge_list_[i].y_min_ != y)
				{
					active_edge_list_[i].x_of_y_min_ += active_edge_list_[i].slash_slope_;
					active_edge_list_[i].y_min_ = y;
				}
				i++;
			}
		}

		std::sort(active_edge_list_.begin(), active_edge_list_.end(), [](Edge e1, Edge e2) {return e1.x_of_y_min_ < e2.x_of_y_min_ || (e1.x_of_y_min_ == e2.x_of_y_min_&&e1.slash_slope_ < e2.slash_slope_); });

		for (int i = 0; i < active_edge_list_.size(); i += 2)
		{
			range_within_polygon_area_.push_back(std::pair<int, std::pair<int, int>>(y, std::pair<int, int>(active_edge_list_[i].x_of_y_min_, active_edge_list_[i + 1].x_of_y_min_)));
		}
	}

	void ScanPolygonArea()
	{
		for (int y = edge_table_[0].y_min_; y <= y_max_; y++)
		{
			ConstructAEL(y);
			UpdateAEL(y);
		}
		int num = 0;
		for (int i = 0; i < range_within_polygon_area_.size(); i++)
		{
			num += range_within_polygon_area_[i].second.second - range_within_polygon_area_[i].second.first;
		}
		std::cout << num << std::endl;
	}

private:
	std::vector<Edge> edge_table_;
	std::vector<Edge> active_edge_list_;
	std::vector<std::pair<int, std::pair<int, int>>> range_within_polygon_area_;
	int y_max_ = -1e9;
};