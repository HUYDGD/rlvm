// -*- Mode: C++; tab-width:2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
// vi:tw=80:et:ts=2:sts=2
//
// -----------------------------------------------------------------------
//
// This file is part of RLVM, a RealLive virtual machine clone.
//
// -----------------------------------------------------------------------
//
// Copyright (C) 2011 Elliot Glaysher
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
// -----------------------------------------------------------------------

#ifndef SRC_SYSTEMS_BASE_COLOURFILTEROBJECTDATA_HPP_
#define SRC_SYSTEMS_BASE_COLOURFILTEROBJECTDATA_HPP_

#include <boost/shared_ptr.hpp>

#include "Systems/Base/GraphicsObjectData.hpp"
#include "Systems/Base/Rect.hpp"

class GraphicsObject;
class GraphicsSystem;

class ColourFilterObjectData : public GraphicsObjectData {
 public:
  ColourFilterObjectData(GraphicsSystem& system, const Rect& screen_rect);
  ~ColourFilterObjectData();

  void setRect(const Rect& screen_rect) { screen_rect_ = screen_rect; }

  // Overriden from GraphicsObjectData:
  virtual void render(const GraphicsObject& go, std::ostream* tree);
  virtual int pixelWidth(const GraphicsObject& rendering_properties);
  virtual int pixelHeight(const GraphicsObject& rendering_properties);
  virtual GraphicsObjectData* clone() const;
  virtual void execute();
  virtual bool isAnimation() const;
  virtual void playSet(int set);

 protected:
  virtual boost::shared_ptr<Surface> currentSurface(const GraphicsObject& rp);
  virtual void objectInfo(std::ostream& tree);

 private:
  GraphicsSystem& graphics_system_;

  Rect screen_rect_;
};

#endif  // SRC_SYSTEMS_BASE_COLOURFILTEROBJECTDATA_HPP_
