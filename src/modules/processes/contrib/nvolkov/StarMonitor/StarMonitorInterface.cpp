// ****************************************************************************
// PixInsight Class Library - PCL 02.01.00.0779
// Standard StarMonitor Process Module Version 01.00.05.0050
// ****************************************************************************
// StarMonitorInterface.cpp - Released 2014/01/30 00:00:00 UTC
// ****************************************************************************
// This file is part of the standard StarMonitor PixInsight module.
//
// Copyright (c) 2011-2014 Nikolay Volkov
// Copyright (c) 2003-2014 Pleiades Astrophoto S.L.
//
// Redistribution and use in both source and binary forms, with or without
// modification, is permitted provided that the following conditions are met:
//
// 1. All redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//
// 2. All redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//
// 3. Neither the names "PixInsight" and "Pleiades Astrophoto", nor the names
//    of their contributors, may be used to endorse or promote products derived
//    from this software without specific prior written permission. For written
//    permission, please contact info@pixinsight.com.
//
// 4. All products derived from this software, in any form whatsoever, must
//    reproduce the following acknowledgment in the end-user documentation
//    and/or other materials provided with the product:
//
//    "This product is based on software from the PixInsight project, developed
//    by Pleiades Astrophoto and its contributors (http://pixinsight.com/)."
//
//    Alternatively, if that is where third-party acknowledgments normally
//    appear, this acknowledgment must be reproduced in the product itself.
//
// THIS SOFTWARE IS PROVIDED BY PLEIADES ASTROPHOTO AND ITS CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL PLEIADES ASTROPHOTO OR ITS
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, BUSINESS
// INTERRUPTION; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; AND LOSS OF USE,
// DATA OR PROFITS) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
// ****************************************************************************

#include "StarMonitorInterface.h"
#include "StarMonitorProcess.h"
#include "Voronoi.h"

#include <GL/freeglut.h>

#include <pcl/Console.h>
#include <pcl/FileDialog.h>
#include <pcl/GlobalSettings.h>
#include <pcl/Random.h>
#include <pcl/FileFormat.h>
#include <pcl/FileFormatInstance.h>
#include <pcl/Settings.h>
//#include <pcl/ExternalProcess.h> // for execute Alert Command PID

#define IMAGELIST_MINHEIGHT( fnt )  (8*fnt.Height() + 2)

namespace pcl
{

double width = 100000, height = 100000;
vor::Voronoi * v;
vor::Vertices * ver; // vertex
vor::Vertices * dir; // sm�ry, kter�mi se pohybuj�
vor::Edges * edg; // edge diagram

bool showVoronoi = true;
bool showDelaunay = true;
bool flatDelaunay = false;
bool moveAll = false;
bool cameraFixZ = false;
bool cameraFixXY = false;
bool demoMode = true;

// ----------------------------------------------------------------------------

StarMonitorInterface* TheStarMonitorInterface = 0;

#define DATATREE_MINHEIGHT( fnt )   (12*fnt.Height() + 2)

// ----------------------------------------------------------------------------

//#include "StarMonitorIcon.xpm"

StarMonitorInterface::StarMonitorInterface() :
ProcessInterface(), instance(TheStarMonitorProcess), GUI(0), files()
{
   TheStarMonitorInterface = this;

   // The auto save geometry feature is of no good to interfaces that include
   // both auto-expanding controls (e.g. TreeBox) and collapsible sections
   // (e.g. SectionBar).
   DisableAutoSaveGeometry();
}

StarMonitorInterface::~StarMonitorInterface()
{
   if (GUI != 0)
      delete GUI, GUI = 0;
}

IsoString StarMonitorInterface::Id() const
{
   return "StarMonitor";
}

MetaProcess* StarMonitorInterface::Process() const
{
   return TheStarMonitorProcess;
}

/*
const char** StarMonitorInterface::IconImageXPM() const
{
   return StarMonitorIcon_XPM;
}
 */
InterfaceFeatures StarMonitorInterface::Features() const
{
   return InterfaceFeature::Default;
}

void StarMonitorInterface::ApplyInstance() const
{
   instance.LaunchOnCurrentView();
}

void StarMonitorInterface::ResetInstance()
{
   StarMonitorInstance defaultInstance(TheStarMonitorProcess);
   ImportProcess(defaultInstance);
}

bool StarMonitorInterface::Launch(const MetaProcess& P, const ProcessImplementation*, bool& dynamic, unsigned& /*flags*/)
{
   // ### Deferred initialization
   if (GUI == 0)
   {
      GUI = new GUIData(*this);
      SetWindowTitle("StarMonitor");

      // Restore position only
      if (!RestoreGeometry())
         SetDefaultPosition();
      AdjustToContents();
   }

   dynamic = false;
   return &P == TheStarMonitorProcess;
}

ProcessImplementation* StarMonitorInterface::NewProcess() const
{
   return new StarMonitorInstance(instance);
}

bool StarMonitorInterface::ValidateProcess(const ProcessImplementation& p, pcl::String& whyNot) const
{
   const StarMonitorInstance* r = dynamic_cast<const StarMonitorInstance*> (&p);

   if (r == 0)
   {
      whyNot = "Not a StarMonitor instance.";
      return false;
   }

   whyNot.Clear();
   return true;
}

bool StarMonitorInterface::RequiresInstanceValidation() const
{
   return true;
}

bool StarMonitorInterface::ImportProcess(const ProcessImplementation& p)
{
   instance.Assign(p);
   UpdateControls();
   return true;
}

void StarMonitorInterface::SaveSettings() const
{
   #if debug
   Console().WriteLn("SaveSettings()");
   #endif
   Settings::Write(SettingsKey() + "_alertExecute", instance.alertExecute);
   Settings::Write(SettingsKey() + "_alertArguments", instance.alertArguments);
}

void StarMonitorInterface::LoadSettings()
{
   #if debug
   Console().WriteLn("LoadSettings()");
   #endif
   Settings::Read(SettingsKey() + "_alertExecute", instance.alertExecute);
   Settings::Read(SettingsKey() + "_alertArguments", instance.alertArguments);
   UpdateControls(); //for apply loaded settings to GUI
}

// ----------------------------------------------------------------------------

void StarMonitorInterface::UpdateControls()
{
   #if debug
   Console().WriteLn("UpdateControls()");
   #endif
   Stop();
   GUI->InputHints_Edit.SetText(instance.inputHints);
   GUI->StructureLayers_SpinBox.SetValue(instance.structureLayers);
   GUI->NoiseLayers_SpinBox.SetValue(instance.noiseLayers);
   GUI->HotPixelFilterRadius_SpinBox.SetValue(instance.hotPixelFilterRadius);
   GUI->Sensitivity_NumericControl.SetValue(Log(instance.sensitivity));
   GUI->PeakResponse_NumericControl.SetValue(instance.peakResponse);
   GUI->MaxStarDistortion_NumericControl.SetValue(instance.maxStarDistortion);
   GUI->Invert_CheckBox.SetChecked(instance.invert);

   GUI->ScaleMode_ComboBox.SetCurrentItem(instance.scaleMode);
   GUI->ScaleValue_NumericEdit.SetValue(instance.scaleValue);
   GUI->ScaleValue_NumericEdit.Enable(instance.scaleMode == SMScaleMode::LiteralValue);

   GUI->Alert_SectionBar.SetChecked(instance.alertCheck);
   GUI->AlertExecute_Edit.SetText(instance.alertExecute);
   GUI->AlertArguments_Edit.SetText(instance.alertArguments);
   GUI->AlertFWHMx_CheckBox.SetChecked(instance.alertFWHMxCheck);
   GUI->AlertFWHMx_NumericControl.SetValue(instance.alertFWHMx);
   GUI->AlertRoundness_CheckBox.SetChecked(instance.alertRoundnessCheck);
   GUI->AlertRoundness_NumericControl.SetValue(instance.alertRoundness);
   GUI->AlertBackground_CheckBox.SetChecked(instance.alertBackgroundCheck);
   GUI->AlertBackground_NumericControl.SetValue(instance.alertBackground);
   GUI->AlertStarQuantity_CheckBox.SetChecked(instance.alertStarQuantityCheck);
   GUI->AlertStarQuantity_NumericControl.SetValue(instance.alertStarQuantity);

   GUI->ShowDelaunay_CheckBox.SetChecked(showDelaunay);
   GUI->Flat_CheckBox.SetChecked(flatDelaunay);
   GUI->ShowVoronoi_CheckBox.SetChecked(showVoronoi);
   GUI->MoveAll_CheckBox.SetChecked(moveAll);
   GUI->CameraFixZ_CheckBox.SetChecked(cameraFixZ);
   GUI->CameraFixXY_CheckBox.SetChecked(cameraFixXY);

   UpdateTargetImagesList();
   CheckPath(); // call for AutoStart monitoring if folder selected.

}

void StarMonitorInterface::AdjustDataTreeColumns()
{
   GUI->TargetImages_TreeBox.DisableHeaderSorting();
   for (int i = 0, n = GUI->TargetImages_TreeBox.NumberOfColumns(); i < n; ++i)
      GUI->TargetImages_TreeBox.AdjustColumnWidthToContents(i);

   GUI->TargetImages_TreeBox.SetColumnWidth(GUI->TargetImages_TreeBox.NumberOfColumns() - 1, 0);
   GUI->TargetImages_TreeBox.EnableHeaderSorting();
}

void StarMonitorInterface::UpdateTargetImagesList()
{
   GUI->TargetImages_TreeBox.DisableUpdates();
   GUI->TargetImages_TreeBox.Clear();
   bool d = instance.targets.IsEmpty();

   GUI->fileClose_Button.Disable(d);
   GUI->fileCloseAll_Button.Disable(d);
   GUI->fileCopyTo_Button.Disable(d);

   for (size_type i = 0; i < instance.targets.Length(); ++i)
   {
      TreeBox::Node* node = new TreeBox::Node(GUI->TargetImages_TreeBox);
      const StarMonitorInstance::Item& item = instance.targets[i];

      float xScale, yScale;
      String s = "''";

      if (instance.scaleMode == SMScaleMode::StandardKeywords)
      {
         xScale = item.xScale;
         yScale = item.yScale;
      }
      else if (instance.scaleMode == SMScaleMode::LiteralValue)
      {
         xScale = yScale = instance.scaleValue;
      }
      else
      {
         xScale = yScale = 1.0;
         s = "px";
      }

      node->SetText(0, String(i));
      node->SetText(1, item.path);
      node->SetText(2, String(item.qty));
      node->SetText(3, String(item.B));
      node->SetText(4, String(item.A));
      node->SetText(5, String(item.sx));
      node->SetText(6, String(item.sy));
      node->SetText(7, String().Format("%.2f", item.FWHMx * xScale) + s);
      node->SetText(8, String().Format("%.2f", item.FWHMy * yScale) + s);
      node->SetText(9, String(item.sy / item.sx));
      node->SetText(10, String(item.theta));
      node->SetText(11, String(item.mad));
   }

   AdjustDataTreeColumns();
   GUI->TargetImages_TreeBox.EnableUpdates();
}

inline bool IsDir(const String dir)
{
   if (dir.IsEmpty() || !File::DirectoryExists(dir))
   {
#ifdef debug
      Console().WriteLn("<br>IsDir() Dir NotSet or NonExists");
#endif
      return false;
   }
   return true;
}

inline bool IsFileGood(const FindFileInfo& info)
{
#ifdef debug
   Console().Write("<br>Checking file: " + info.name);
#endif
   if (info.IsEmpty())
   {
#ifdef debug
      Console().Write(" IsEmpty, skip.");
#endif
      return false; // skip empty files
   }
   if (info.IsTemporary())
   {
#ifdef debug
      Console().Write(" IsTemporary, skip.");
#endif
      return false; // skip Temporary files
   }
   return true;
}

void StarMonitorInterface::InitFindFile() // Store all filenames.fit from monitoredFolder to "StringList files"
{
   files.Clear();
   if (!IsDir(instance.monitoredFolder)) return;
   String s = instance.monitoredFolder + "/" + "*.fit";

   File::Find find(s);
   FindFileInfo info;

   while (find.NextItem(info))
   {
      if (IsFileGood(info))
      {
#ifdef debug
         Console().Write(" is OK, store.");
#endif
         files.Add(info.name);
      }
   }
#ifdef debug
   Console().WriteLn(String().Format("<br>Total %u filename stored in ignoring table.", files.Length()));
#endif
}

void StarMonitorInterface::ProcessFile(const String& path)
{
   Disable(); // Diasable GUI during PSF processing
   if (!instance.ProcessFile(path)) // Call Load file and PSF processing
   {
      Console().WriteLn("To Do: Write exception code ");
   }
   else //The file sucesfull processed and added to targets
   {
      UpdateTargetImagesList();
   }
   Enable(); // Enable GUI
}

void StarMonitorInterface::FindNewFile()
{
   if (!IsDir(instance.monitoredFolder)) return;
   String s = instance.monitoredFolder + "/" + "*.fit";

   File::Find find(s);
   FindFileInfo info;

   while (find.NextItem(info))
   {
      if (!IsFileGood(info)) continue; // skip bad( empty, Temporary,... ) files

#ifdef debug
      Console().Write(" is OK, finding in stored table.");
#endif

      size_t i = 0;
      while (i < files.Length() && info.name != files[i]) // Compare filename form folder with stored filenames in "StringList files"
         i++;

      if (i < files.Length())
      {
#ifdef debug
         Console().Write(" found, skip old file.");
#endif
         continue; // the file is old, skip old files
      }

#ifdef debug
      Console().WriteLn(" is new, start processing.");
#endif

      files.Add(info.name); //store new filename in "StringList files"
      String path = instance.monitoredFolder + "/" + info.name;
      ProcessFile(path);
      CheckSkyCondition();
   }

#ifdef debug
   Console().WriteLn(String().Format("<br>Total %u filename stored in ignoring table.", files.Length()));
#endif
}

void StarMonitorInterface::Stop()
{
   GUI->UpdateRealTime_Timer.Stop();
   GUI->MonitoringAnimation.Hide();
   GUI->MonitoringStartStop_ToolButton.SetIcon(Bitmap(String(":/images/activate_view.png")));
   GUI->MonitoringStartStop_ToolButton.Uncheck();
}

void StarMonitorInterface::CheckPath(bool tryStart)
{
   if (tryStart) GUI->MonitoringStartStop_ToolButton.SetChecked();
   if (!IsDir(instance.monitoredFolder))
   {
      Stop();
      GUI->MonitoringStartStop_ToolButton.Disable();
   }
   else
   {
      GUI->MonitoringStartStop_ToolButton.Enable();
      if (GUI->MonitoringStartStop_ToolButton.IsChecked())
      {
         InitFindFile();
         GUI->MonitoringStartStop_ToolButton.SetIcon(Bitmap(String(":/images/pause.png")));
         GUI->MonitoringAnimation.Show();
         GUI->UpdateRealTime_Timer.Start();
      }
      else Stop();
   }

   GUI->MonitoredFolder_Edit.SetText(instance.monitoredFolder);
}

void StarMonitorInterface::CheckSkyCondition()
{
   if (!instance.alertCheck) return;

   size_t lastFile = instance.targets.Length();

#ifdef debug
   Console().WriteLn("CheckSkyCondition() start<flash>");
   Console().WriteLn("Total files in TreeBox: " + String(lastFile));
#endif

   if (lastFile == 0) return;
   lastFile--;

   StarMonitorInstance::Item* item = &instance.targets[lastFile];
   bool alert = false;

   #ifdef debug
   Console().WriteLn("CheckSkyCondition() Check file: " + item->path);
#endif

   if (instance.alertFWHMxCheck && (instance.alertFWHMx < item->FWHMx))
   {
      #ifdef debug
      Console().WriteLn("alertFWHMx");
#endif

      alert = true;
   }
   else if (instance.alertRoundnessCheck && (instance.alertRoundness > (item->FWHMy / item->FWHMx)))
   {
      #ifdef debug
      Console().WriteLn("alertRoundness");
#endif

      alert = true;
   }
   else if (instance.alertBackgroundCheck && (instance.alertBackground < item->B))
   {
      #ifdef debug
      Console().WriteLn("alertBackground");
#endif

      alert = true;
   }
   else if (instance.alertStarQuantityCheck && (instance.alertStarQuantity > item->qty))
   {
      #ifdef debug
      Console().WriteLn("alertStarQuantity");
#endif

      alert = true;
   }
   else
   {
      #ifdef debug
      Console().WriteLn("SkyCondition is OK. No alert");
      #endif
   }

   if (alert) Alert();
}

void StarMonitorInterface::Alert()
{
   Console().WriteLn("Alert detected!!!");
   
   if (GUI->my_AlertProcess.IsRunning())
   {
		Console().WriteLn("Alert process is already running.");
	   #ifndef __PCL_WINDOWS
		Console().WriteLn("PID=" + GUI->my_AlertProcess.PID());
		#endif
      return;
   }

   StringList arguments;
   instance.alertArguments.Break(arguments, ' ', true); //break 1 string to separate argument strings
   Console().WriteLn("Start Alert command: " + instance.alertExecute + " " + instance.alertArguments);
   GUI->Log_TextBox.Clear();
   try
   {
      GUI->my_AlertProcess.Start(instance.alertExecute, arguments); //Execute external processing
   }
   catch (...)
   {
      throw;
   }
}

inline void StarMonitorInterface::FileAdd()
{
   OpenFileDialog d;
   d.LoadImageFilters();
   d.EnableMultipleSelections();
   d.SetCaption("StarMonitor: Add files for calculating Star Statistics");
   if (d.Execute())
   {
      for (size_t i = 0; i < d.FileNames().Length(); i++)
      {
         String path = d.FileNames()[i];
         ProcessFile(path);
      }
#ifdef debug
      Console().WriteLn("Debug Mode: CheckSkyCondition for one last loaded file");
      CheckSkyCondition();
#endif

   }
}

String StarMonitorInterface::UniqueFilePath(const String& fileName, const String& dir)
{
   String filePath = dir + "/" + File::ExtractName(fileName) + File::ExtractExtension(fileName);
   if (!File::Exists(filePath))
      return filePath;

   for (unsigned u = 1;; ++u)
   {
      String tryFilePath = File::AppendToName(filePath, '_' + String(u));
      if (!File::Exists(tryFilePath))
         return tryFilePath;
   }
}

void StarMonitorInterface::FileCopyTo()
{
   //Pause();

   const String dir(SelectDirectory("StarMonitor/FileCopy: Select Output Directory"));
   if (dir.IsEmpty())
   {
      //Continue();
      return;
   }

   try
   {
      Console().Show();

      Console().WriteLn("<end><cbr><br>StarMonitor: " + String(GUI->TargetImages_TreeBox.SelectedNodes().Length()) + " files selected.");
      for (int row = 0; row < GUI->TargetImages_TreeBox.NumberOfChildren(); row++)
         if (GUI->TargetImages_TreeBox.Child(row)->IsSelected())
         {
            const String fromFilePath = GUI->TargetImages_TreeBox.Child(row)->Text(0);
            const String toFilePath = UniqueFilePath(fromFilePath, dir);
            Console().WriteLn("Copy to " + toFilePath);
            ProcessEvents();
            File::WriteFile(toFilePath, File::ReadFile(fromFilePath));
         }
      Console().WriteLn("Done.");

      Console().Hide();
      //Continue();
   }
   catch (...)
   {
      Console().Hide();
      //Stop();
      throw;
   }
}

String StarMonitorInterface::SelectDirectory(const String& caption, const String& initialPath)
{
   GetDirectoryDialog d;
   d.SetCaption(caption);
   d.SetInitialPath(initialPath);
   if (d.Execute())
      return d.Directory();
   return String();
}

/*
 *		3D Show engine
 */

inline void draw()
{
   //draw x,y,z axis in center
   glColor3f(1, 0, 0); //Red
   glBegin(GL_LINES);
   glVertex3f(0.0, 0.0, 0.0);
   glVertex3f(GLfloat(0.1), 0.0, 0.0);
   glVertex3f(0.0, 0.0, 0.0);
   glVertex3f(0.0, GLfloat(0.1), 0.0);
   glVertex3f(0.0, 0.0, 0.0);
   glVertex3f(0.0, 0.0, GLfloat(0.1));
   glEnd();

   glColor3f(0, 1, 0); //Green
   // draw rectangle border around diagram area
   glBegin(GL_LINE_LOOP);
   glVertex2f(-1, -1);
   glVertex2f(-1, 1);
   glVertex2f(1, 1);
   glVertex2f(1, -1);
   glEnd();

   // draw vertical in 4 conners
   glBegin(GL_LINES);
   glVertex3f(-1, -1, 0);
   glVertex3f(-1, -1, .5);
   glVertex3f(-1, 1, 0);
   glVertex3f(-1, 1, .5);
   glVertex3f(1, 1, 0);
   glVertex3f(1, 1, .5);
   glVertex3f(1, -1, 0);
   glVertex3f(1, -1, .5);
   glEnd();


   // draw Rect in center of Vertices
   for (vor::Vertices::iterator i = ver->begin(); i != ver->end(); ++i)
   {
      glColor3f((*i)->c, (*i)->c - 1, (*i)->c - 1);
      glRectf(-1 + 2 * (*i)->x / width - 0.01, -1 + 2 * (*i)->y / height - 0.01, -1 + 2 * (*i)->x / width + 0.01, -1 + 2 * (*i)->y / height + 0.01);
   }

   // draw sphere in center of movment Vertices
   if (demoMode)
   {
      int k = 0;
      for (vor::Vertices::iterator i = ver->begin(); i != ver->end() && k <= 10; ++i, k++)
      {
         glPushMatrix();
         glTranslatef(-1 + 2 * (*i)->x / width, -1 + 2 * (*i)->y / height, (*i)->c / 5 + .2);
         glColor3f((*i)->c, 1 - (*i)->c, 1 - (*i)->c);
         glutWireSphere(0.02, 10, 10);
         glPopMatrix();
      }
   }

   // Draw diagrams
   glBegin(GL_LINES);
   if (showVoronoi)
   {
      glColor3f(0, 0.5, 0);
      for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i)
      {
         glVertex2f(-1 + 2 * (*i)->start->x / width, -1 + 2 * (*i)->start->y / height);
         glVertex2f(-1 + 2 * (*i)->end->x / width, -1 + 2 * (*i)->end->y / height);
      }
   }
   if (showDelaunay)
   {
      if (flatDelaunay) for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i)
         {
            glColor3f((*i)->left->c, 1 - (*i)->left->c, 1 - (*i)->left->c);
            glVertex2f(-1 + 2 * (*i)->left->x / width, -1 + 2 * (*i)->left->y / height);
            glColor3f((*i)->right->c, 1 - (*i)->right->c, 1 - (*i)->right->c);
            glVertex2f(-1 + 2 * (*i)->right->x / width, -1 + 2 * (*i)->right->y / height);
         }
      else for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i)
         {
            glColor3f((*i)->left->c, 1 - (*i)->left->c, 1 - (*i)->left->c);
            glVertex3f(-1 + 2 * (*i)->left->x / width, -1 + 2 * (*i)->left->y / height, (*i)->left->c / 5 + .2);
            glColor3f((*i)->right->c, 1 - (*i)->right->c, 1 - (*i)->right->c);
            glVertex3f(-1 + 2 * (*i)->right->x / width, -1 + 2 * (*i)->right->y / height, (*i)->right->c / 5 + .2);
         }
   }
   glEnd();
}

void onEF(int n)
{
   glutTimerFunc(40, onEF, 0); // Call byself after 20ms

   static double cameraXY = 0, cameraZ = 0;

   glLoadIdentity(); // Load the Identity Matrix to reset our drawing locations
   glTranslatef(0.0f, 0.0f, -5.0f);
   gluLookAt(sin(cameraXY), cos(cameraXY), 0.2 * sin(cameraZ) + .3, 0, 0, 0, 0, 0, 1); // Look from Camera[x,y,z] to [0,0,0]; orient z-axiz up
   if (!cameraFixXY) cameraXY += 0.01; // move camera hirizontaly
   if (!cameraFixZ) cameraZ += 0.1; // move camera vierticaly

   glClear(GL_COLOR_BUFFER_BIT); // Clear the screen
   glClearColor(0.0f, 0.0f, 0.2f, 1.0f); // Clear the background of our window to red

   if (demoMode) // move v (Vertices) according dir (Derection)
   {
      int k = 0;
      for (vor::Vertices::iterator i = ver->begin(), j = dir->begin(); i != ver->end(); ++i, ++j)
      {
         (*i)->x += (*j)->x;
         if ((*i)->x < 0 || (*i)->x > width) (*j)->x *= -1;

         (*i)->y += (*j)->y;
         if ((*i)->y < 0 || (*i)->y > height) (*j)->y *= -1;
         k++;
         if (!moveAll && k >= 10) break; // limit qty of moving Vertices to 10
      }
      edg = v->GetEdges(ver, width, height); // calculate new diagram
   }

   draw();

   glutSwapBuffers(); // Show everything to the screen
   glutPopWindow(); // popping to top-level windows inside PixInsight
}

void display(void)
{
   // All magic doing in onEF() called via glutTimerFunc()
}

void reshape(int w, int h)
{
   glViewport(0, 0, (GLsizei) w, (GLsizei) h); // Set our viewport to the size of our window
   glMatrixMode(GL_PROJECTION); // Switch to the projection matrix so that we can manipulate how our scene is viewed
   glLoadIdentity(); // Reset the projection matrix to the identity matrix so that we don't get any artifacts (cleaning up)
   gluPerspective(20, (GLfloat) w / (GLfloat) h, 1.0, 100.0); // Set the Field of view angle (in degrees), the aspect ratio of our window, and the new and far planes
   glMatrixMode(GL_MODELVIEW); // Switch back to the model view matrix, so that we can start drawing shapes correctly
}

void LoadImage(Image* img, const String& filePath)
{
   Console().WriteLn("<end><cbr>Loading image:" + filePath);
   FileFormat format(File::ExtractExtension(filePath), true, false);
   FileFormatInstance file(format);
   ImageDescriptionArray images;
   if (!file.Open(images, filePath)) throw CatchedException();
   if (images.IsEmpty()) throw Error(filePath + ": Empty image.");
   if (images.Length() > 1) throw Error(filePath + ": Multiple images cannot be used.");
   if (!file.SelectImage(0)) throw CatchedException();
   if (!file.ReadImage(*img)) throw CatchedException();
   file.Close();
}

inline void InitGLDemo(void)
{
   using namespace vor;
   dir = new Vertices(); // list of <VPoint*>
   RandomNumberGenerator rD;

   for (int i = 0; i < 100; i++)
   {
      double x = rD(), y = rD();
      double z = (x - 0.5)*(y - 0.5);
      ver->push_back(new VPoint(x*width, y*height, z * z * 32 + 0.1 * rD()));
      dir->push_back(new VPoint(rD.Gaussian(0, double(width) / 600.0), rD.Gaussian(0, double(width) / 600.0)));
   }
}

inline void StarMonitorInterface::InitGL(int nodeIndex)
{
   using namespace vor;
   StarMonitorInstance::Item* item = &instance.targets[GUI->TargetImages_TreeBox.Child(nodeIndex)->Text(0).ToInt()];

   Image* img = new Image((void*) 0, 0, 0);
   LoadImage(img, item->path);
   const double iWidth = img->Width();
   const double iHeight = img->Height();
   delete img;

   const double FWHM_Min = item->FWHM_Min;
   const double FWHM_Max = item->FWHM_Max;
   const double FWHM_AVR = (item->FWHMx + item->FWHMx) / 2;
   const double k = 2 / (FWHM_Max - FWHM_Min); // *2 == power

   for (Array<Star>::iterator i = item->stars.Begin(); i != item->stars.End(); ++i)
   {
      double fwhm = (Max(i->psf.FWHMx(), i->psf.FWHMy()) - FWHM_AVR) * k;
      ver->push_back(new VPoint(i->pos.x / iWidth*width, i->pos.y / iHeight*height, fwhm));
   }
}

inline void StarMonitorInterface::Show3D(int index)
{
   v = new vor::Voronoi();
   ver = new vor::Vertices();

   demoMode = bool( index < 0);
   if (demoMode) InitGLDemo();
   else InitGL(index);

   edg = v->GetEdges(ver, width, height);

#ifdef debug
   Console().WriteLn("voronois done!");

   for (vor::Edges::iterator i = edg->begin(); i != edg->end(); ++i)
   {
      if ((*i)->start == 0)
      {
         Console().WriteLn("missing edges beginning!");
         continue;
      }
      if ((*i)->end == 0)
      {
         Console().WriteLn("missing end edges!");
         continue;
      }
   }
#endif

   // init Glut

   int argc = 1;
   char *argv[] = {IsoString().c_str(), NULL};
   //char *argv[] = { ' ', NULL };
   glutInit(&argc, argv);

   glutInitDisplayMode(GLUT_DOUBLE); // Set up a basic display buffer
   glutInitWindowSize(800, 600); // Set the width and height of the window
   glutInitWindowPosition(100, 100); // Set the position of the window
   glutCreateWindow("OpenGL Window"); // Set the title for the window
   Console().WriteLn("Created 'OpenGL Window'");

   glutTimerFunc(100, onEF, 0);
   gluOrtho2D(-1.0, 1.0, -1.0, 1.0); // set system coordinates
   glutDisplayFunc(display); // Tell GLUT to use the method "display" for rendering
   glutReshapeFunc(reshape); // Tell GLUT to use the method "reshape" for reshaping

   glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS); // Dear GLUT, please don't close PixInsight on exit from glutMainLoop

   glutMainLoop(); // Run main animation loop
   Console().WriteLn("OpenGL MainLoop terminated.");
}

/*
 * Event Handlers -----------------------------------------------------------------
 */
void StarMonitorInterface::__EditCompleted(Edit& sender)
{
   String text = sender.Text().Trimmed();
   
   if (sender == GUI->AlertArguments_Edit)
   {
      instance.alertArguments = text;
   }
   else if (sender == GUI->AlertExecute_Edit)
   {
      instance.alertExecute = text;
   }
   else if (sender == GUI->MonitoredFolder_Edit)
   {
      instance.monitoredFolder = text;
      CheckPath();
   }
   else if (sender == GUI->InputHints_Edit)
   {
      instance.inputHints = text;
   }
   else if (sender == GUI->StdIn_Edit)
   {
      if (GUI->my_AlertProcess.IsRunning() && !text.IsEmpty())
      {
         String s(text + "<br>");
         AddLog(s);
         GUI->my_AlertProcess.Write(text);
         GUI->my_AlertProcess.CloseStandardInput();
         text.Clear();
      }
   }
   sender.SetText(text);
}

void StarMonitorInterface::__Button_Click(Button& sender, bool checked)
{
   // Select MonitoredFolder Dialog -----------------------------
   if (sender == GUI->MonitoredFolder_ToolButton)
   {
      const String dir(SelectDirectory("StarMonitor: Select monitored folder"));
      if (!dir.IsEmpty())
      {
         instance.monitoredFolder = dir;
         CheckPath();
      }
   } // Run/Stop Monitoring -----------------------------------
   else if (sender == GUI->MonitoringStartStop_ToolButton)
      CheckPath(false);

   else if (sender == GUI->Invert_CheckBox)
      instance.invert = checked;

      // Add file(s) ----------------------------------------
   else if (sender == GUI->fileAdd_Button)
      FileAdd();

      // Close selected file(s) ----------------------------------------
   else if (sender == GUI->fileClose_Button)
   {
      Disable();

      for (int i = 0; i < GUI->TargetImages_TreeBox.NumberOfChildren(); i++)
      {
         if (!GUI->TargetImages_TreeBox.Child(i)->IsSelected()) continue;

         String p1 = GUI->TargetImages_TreeBox.Child(i)->Text(0);

#ifdef debug
         Console().WriteLn("Selected file in TreeBox:" + p1);
#endif

         for (size_t j = 0; j < instance.targets.Length(); j++)
         {
            if (p1 != instance.targets[j].path) continue;

#ifdef debug
            Console().WriteLn("Found same file in targets:" + instance.targets[j].path);
#endif

            instance.targets.Remove(instance.targets.At(j));

#ifdef debug
            Console().WriteLn("File removed.");
#endif

            break;
         }
      }
      UpdateTargetImagesList();
      Enable();
   } // Close All file(s) ----------------------------------------
   else if (sender == GUI->fileCloseAll_Button)
   {
      instance.targets.Clear();
      GUI->TargetImages_TreeBox.Clear();
   }// Copy selected filse to ----------------------------------------
   else if (sender == GUI->fileCopyTo_Button)
   {
      FileCopyTo();
   } // Alert: Enable/Disable  ---------------------------------------------
   else if (sender == GUI->AlertFWHMx_CheckBox) instance.alertFWHMxCheck = checked;
   else if (sender == GUI->AlertRoundness_CheckBox) instance.alertRoundnessCheck = checked;
   else if (sender == GUI->AlertBackground_CheckBox) instance.alertBackgroundCheck = checked;
   else if (sender == GUI->AlertStarQuantity_CheckBox) instance.alertStarQuantityCheck = checked;

      // Alert: Execute Test ---------------------------------------------
   else if (sender == GUI->AlertArgumentsTest_ToolButton)
   {
      
      __EditCompleted(GUI->AlertArguments_Edit); //force store text to instance, because tool button don't call it automatically
      __EditCompleted(GUI->AlertExecute_Edit);
      Alert();
   }
   else if (sender == GUI->Show3D_Button)
      Show3D(GUI->TargetImages_TreeBox.ChildIndex(GUI->TargetImages_TreeBox.CurrentNode()));
   else if (sender == GUI->ShowVoronoi_CheckBox)
      showVoronoi = checked;
   else if (sender == GUI->ShowDelaunay_CheckBox)
      showDelaunay = checked;
   else if (sender == GUI->Flat_CheckBox)
      flatDelaunay = checked;
   else if (sender == GUI->MoveAll_CheckBox)
      moveAll = checked;
   else if (sender == GUI->CameraFixZ_CheckBox)
      cameraFixZ = checked;
   else if (sender == GUI->CameraFixXY_CheckBox)
      cameraFixXY = checked;
}

void StarMonitorInterface::__CheckSection(SectionBar& sender, bool checked)
{
   // Alert: Enable/Disable  ---------------------------------------------
   if (sender == GUI->Alert_SectionBar) instance.alertCheck = checked;
}

void StarMonitorInterface::__StarDetection_SpinValueUpdated(SpinBox& sender, int value)
{
   if (sender == GUI->StructureLayers_SpinBox)
      instance.structureLayers = value;
   else if (sender == GUI->NoiseLayers_SpinBox)
      instance.noiseLayers = value;
   else if (sender == GUI->HotPixelFilterRadius_SpinBox)
      instance.hotPixelFilterRadius = value;
}

void StarMonitorInterface::__StarDetection_EditValueUpdated(NumericEdit& sender, double value)
{
   if (sender == GUI->Sensitivity_NumericControl)
      instance.sensitivity = Pow10(value);
   else if (sender == GUI->PeakResponse_NumericControl)
      instance.peakResponse = value;
   else if (sender == GUI->MaxStarDistortion_NumericControl)
      instance.maxStarDistortion = value;
}

void StarMonitorInterface::__ItemSelected(ComboBox& sender, int itemIndex)
{
   if (sender == GUI->ScaleMode_ComboBox)
   {
      instance.scaleMode = itemIndex;
      UpdateControls();
      //UpdateScaleItems();
   }
}

void StarMonitorInterface::__NumericEdit_ValueUpdated(NumericEdit& sender, double value)
{
   if (sender == GUI->ScaleValue_NumericEdit)
   {
      instance.scaleValue = value;
      UpdateControls();
   }
   else if (sender == GUI->AlertFWHMx_NumericControl) instance.alertFWHMx = value;
   else if (sender == GUI->AlertRoundness_NumericControl) instance.alertRoundness = value;
   else if (sender == GUI->AlertBackground_NumericControl) instance.alertBackground = value;
   else if (sender == GUI->AlertStarQuantity_NumericControl) instance.alertStarQuantity = value;
}

inline void StarMonitorInterface::__Timer(Timer& t)
{
   static int i = 1;
   if (i < 8) i++;
   else
   {
      i = 1;
      FindNewFile();
   }
   //String p = String().Format(":/images/real_time_preview/realTimeBusy0%u.png", i );
   GUI->MonitoringAnimation.SetIcon(Bitmap(String().Format(":/images/real_time_preview/realTimeBusy0%u.png", i)));
   t.Start();
}

   
void StarMonitorInterface::AddLog(String s)
{
   GUI->Log_TextBox.SetText(GUI->Log_TextBox.Text() + s);
}

void StarMonitorInterface::__ExternalProcessOnFinished(ExternalProcess& p, int exitCode, bool exitOk)
{
   AddLog("<end><br>Alert processing finished.");
   GUI->StdIn_Edit.Disable();
}

void StarMonitorInterface::__ExternalProcessOnStarted(ExternalProcess& p)
{
   AddLog("Alert processing started.<br>");
   GUI->StdIn_Edit.Enable();
   //GUI->StdIn_Edit.Focus();
}

void StarMonitorInterface::__ExternalProcessOnStandardOutputDataAvailable(ExternalProcess& p)
{
   ByteArray d(p.Read());
   String s = String::UTF8ToUTF16(reinterpret_cast<const char*> (d.Begin()), 0, d.Length());
   AddLog(s);
}

void StarMonitorInterface::__ExternalProcessOnError(ExternalProcess& p, ExternalProcess::error_code error)
{
   const char *str[] = {"FailedToStart", "Crashed", "TimedOut", "ReadError", "WriteError", "UnknownError"};
   AddLog("<end><br>ExternalProcess Error:" + String(str[error]));
   //DisableGUI(false);
}


StarMonitorInterface::GUIData::GUIData(StarMonitorInterface& w)
{
   pcl::Font fnt = w.Font();
   int editWidth2 = fnt.Width(String('0', 11));
   int labelWidth1 = fnt.Width(String("Image scale (arcsec/px):") + 'T');

   //

   TargetImages_TreeBox.SetMinHeight(DATATREE_MINHEIGHT(w.Font()));
   TargetImages_TreeBox.SetNumberOfColumns(13);
   TargetImages_TreeBox.SetHeaderText(0, "#");
   TargetImages_TreeBox.SetHeaderText(1, "File");
   TargetImages_TreeBox.SetHeaderText(2, "Stars");
   TargetImages_TreeBox.SetHeaderText(3, "Background");
   TargetImages_TreeBox.SetHeaderText(4, "Amplitude");
   TargetImages_TreeBox.SetHeaderText(5, "SigmaX");
   TargetImages_TreeBox.SetHeaderText(6, "SigmaY");
   TargetImages_TreeBox.SetHeaderText(7, "FWHMx");
   TargetImages_TreeBox.SetHeaderText(8, "FWHMy");
   TargetImages_TreeBox.SetHeaderText(9, "Roundnes");
   TargetImages_TreeBox.SetHeaderText(10, "Rotation");
   TargetImages_TreeBox.SetHeaderText(11, "MAD");
   TargetImages_TreeBox.SetHeaderText(12, String());

   for (int i = 0; i < TargetImages_TreeBox.NumberOfColumns(); ++i)
      TargetImages_TreeBox.SetHeaderAlignment(i, TextAlign::Center | TextAlign::VertCenter);

   TargetImages_TreeBox.DisableRootDecoration();
   TargetImages_TreeBox.EnableAlternateRowColor();
   TargetImages_TreeBox.EnableMultipleSelections();

   /*
      TargetImages_TreeBox.OnCurrentNodeUpdated( (TreeBox::node_navigation_event_handler)&DynamicPSFInterface::__CurrentNodeUpdated, w );
      TargetImages_TreeBox.OnNodeActivated( (TreeBox::node_event_handler)&DynamicPSFInterface::__NodeActivated, w );
      TargetImages_TreeBox.OnNodeDoubleClicked( (TreeBox::node_event_handler)&DynamicPSFInterface::__NodeDoubleClicked, w );
      TargetImages_TreeBox.OnNodeExpanded( (TreeBox::node_expand_event_handler)&DynamicPSFInterface::__NodeExpanded, w );
      TargetImages_TreeBox.OnNodeCollapsed( (TreeBox::node_expand_event_handler)&DynamicPSFInterface::__NodeCollapsed, w );
      TargetImages_TreeBox.OnNodeSelectionUpdated( (TreeBox::tree_event_handler)&DynamicPSFInterface::__NodeSelectionUpdated, w );
    */
   //

   MonitoredFolder_Label.SetText("Monitoring Folder:");
   MonitoredFolder_Label.SetFixedWidth(labelWidth1);
   MonitoredFolder_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);

   MonitoredFolder_Edit.OnEditCompleted((Edit::edit_event_handler) & StarMonitorInterface::__EditCompleted, w);

   MonitoredFolder_ToolButton.SetIcon(Bitmap(String(":/images/icons/select.png")));
   MonitoredFolder_ToolButton.SetFixedSize(19, 19);
   MonitoredFolder_ToolButton.SetToolTip("<p>Click to select monitored folder</p>");
   MonitoredFolder_ToolButton.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   MonitoredFolder_Sizer.SetSpacing(4);
   MonitoredFolder_Sizer.Add(MonitoredFolder_Label);
   MonitoredFolder_Sizer.Add(MonitoredFolder_Edit, 100);
   MonitoredFolder_Sizer.Add(MonitoredFolder_ToolButton);

   //

   MonitoringButton_Label.SetText("Start Monitoring:");
   MonitoringButton_Label.SetFixedWidth(labelWidth1);
   MonitoringButton_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);

   MonitoringStartStop_ToolButton.SetIcon(Bitmap(String(":/images/activate_view.png")));
   MonitoringStartStop_ToolButton.SetToolTip("<p>Click to start monitoring folder</p>");
   MonitoringStartStop_ToolButton.SetCheckable();
   MonitoringStartStop_ToolButton.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   MonitoringAnimation.Hide();

   //const char* SelectionNote = "<br><br>Note: Checked not qual to Selected. To select more then one image use Shift or Ctrl + narow keys or Mouse.";
   fileAdd_Button.SetIcon(Bitmap(String(":/images/image_container/add_files.png")));
   fileAdd_Button.SetToolTip("Add image files.<br>");
   fileAdd_Button.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   fileClose_Button.SetIcon(Bitmap(String(":/images/close.png")));
   fileClose_Button.SetToolTip("Close Selected images.");
   fileClose_Button.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   fileCloseAll_Button.SetIcon(Bitmap(String(":/images/close_all.png")));
   fileCloseAll_Button.SetToolTip("Close all images.<br>");
   fileCloseAll_Button.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   fileCopyTo_Button.SetIcon(Bitmap(String(":/images/icons/disks.png")));
   fileCopyTo_Button.SetToolTip("Copy selected files to new location.");
   fileCopyTo_Button.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   MonitoringButton_Sizer.SetSpacing(4);
   MonitoringButton_Sizer.Add(MonitoringButton_Label);
   MonitoringButton_Sizer.Add(MonitoringStartStop_ToolButton);
   MonitoringButton_Sizer.Add(MonitoringAnimation);
   MonitoringButton_Sizer.AddStretch();
   MonitoringButton_Sizer.Add(fileAdd_Button);
   MonitoringButton_Sizer.Add(fileClose_Button);
   MonitoringButton_Sizer.Add(fileCloseAll_Button);
   MonitoringButton_Sizer.Add(fileCopyTo_Button);

   //

   GeneralParameters_Sizer.SetSpacing(4);
   GeneralParameters_Sizer.Add(MonitoredFolder_Sizer);
   GeneralParameters_Sizer.Add(MonitoringButton_Sizer);

   //

   FormatHints_SectionBar.SetTitle("Format Hints");
   FormatHints_SectionBar.SetSection(FormatHints_Control);
   //   FormatHints_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&StarMonitorInterface::__ToggleSection, w );
   /*
      const char* hintsToolTip = "<p><i>Format hints</i> allow you to override global file format settings "
         "for image files used by specific processes. In StarMonitor, input hints change the way input images of "
         "some particular file formats (target and target image files) are loaded, while output hints modify the "
         "way output registered image files are written.</p>"
         "<p>For example, you can use the \"lower-range\" and \"upper-range\" input hints to load floating point FITS and "
         "TIFF files generated by other applications that don't use PixInsight's normalized [0,1] range. Similarly, you "
         "can specify the \"up-bottom\" output hint to write all registered images (in FITS format) with the coordinate "
         "origin at the top-left corner. Most standard file format modules support hints; each format supports a number of "
         "input and/or output hints that you can use for different purposes with tools that give you access to format hints.</p>";
    */
   InputHints_Label.SetText("Input hints:");
   InputHints_Label.SetFixedWidth(labelWidth1);
   InputHints_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
   //   InputHints_Label.SetToolTip( hintsToolTip );

   //   InputHints_Edit.SetToolTip( hintsToolTip );
   InputHints_Edit.OnEditCompleted((Edit::edit_event_handler) & StarMonitorInterface::__EditCompleted, w);

   InputHints_Sizer.SetSpacing(4);
   InputHints_Sizer.Add(InputHints_Label);
   InputHints_Sizer.Add(InputHints_Edit, 100);

   FormatHints_Sizer.SetSpacing(4);
   FormatHints_Sizer.Add(InputHints_Sizer);


   FormatHints_Control.SetSizer(FormatHints_Sizer);
   FormatHints_Control.AdjustToContents();


   StarDetection_SectionBar.SetTitle("Star Detection");
   StarDetection_SectionBar.SetSection(StarDetection_Control);
   //   StarDetection_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&StarMonitorInterface::__ToggleSection, w );

   const char* structureLayersToolTip =
           "<p>Number of wavelet layers used for structure detection.</p>"
           "<p>With more wavelet layers, larger stars (and perhaps also some nonstellar objects) will be detected.</p>";

   StructureLayers_Label.SetText("Detection scales:");
   StructureLayers_Label.SetFixedWidth(labelWidth1);
   StructureLayers_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
   StructureLayers_Label.SetToolTip(structureLayersToolTip);

   StructureLayers_SpinBox.SetRange(int( TheSMStructureLayersParameter->MinimumValue()), int( TheSMStructureLayersParameter->MaximumValue()));
   StructureLayers_SpinBox.SetToolTip(structureLayersToolTip);
   StructureLayers_SpinBox.SetFixedWidth(editWidth2);
   StructureLayers_SpinBox.OnValueUpdated((SpinBox::value_event_handler) & StarMonitorInterface::__StarDetection_SpinValueUpdated, w);

   StructureLayers_Sizer.SetSpacing(4);
   StructureLayers_Sizer.Add(StructureLayers_Label);
   StructureLayers_Sizer.Add(StructureLayers_SpinBox);
   StructureLayers_Sizer.AddStretch();

   const char* noiseLayersToolTip =
           "<p>Number of wavelet layers used for noise reduction.</p>"
           "<p>Noise reduction prevents detection of bright noise structures as false stars, including hot pixels and "
           "cosmic rays. This parameter can also be used to control the sizes of the smallest detected stars (increase "
           "to exclude more stars).</p>";

   NoiseLayers_Label.SetText("Noise scales:");
   NoiseLayers_Label.SetFixedWidth(labelWidth1);
   NoiseLayers_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
   NoiseLayers_Label.SetToolTip(noiseLayersToolTip);

   NoiseLayers_SpinBox.SetRange(int( TheSMNoiseLayersParameter->MinimumValue()), int( TheSMNoiseLayersParameter->MaximumValue()));
   NoiseLayers_SpinBox.SetToolTip(noiseLayersToolTip);
   NoiseLayers_SpinBox.SetFixedWidth(editWidth2);
   NoiseLayers_SpinBox.OnValueUpdated((SpinBox::value_event_handler) & StarMonitorInterface::__StarDetection_SpinValueUpdated, w);

   NoiseLayers_Sizer.SetSpacing(4);
   NoiseLayers_Sizer.Add(NoiseLayers_Label);
   NoiseLayers_Sizer.Add(NoiseLayers_SpinBox);
   NoiseLayers_Sizer.AddStretch();

   const char* hotPixelFilterRadiusToolTip =
           "<p>Size of the hot pixel removal filter.</p>"
           "<p>This is the radius in pixels of a median filter applied by the star detector before the structure "
           "detection phase. A median filter is very efficient to remove <i>hot pixels</i>. Hot pixels will be "
           "identified as false stars, and if present in large amounts, can prevent a valid image registration.</p>"
           "<p>To disable hot pixel removal, set this parameter to zero.</p>";

   HotPixelFilterRadius_Label.SetText("Hot pixel removal:");
   HotPixelFilterRadius_Label.SetFixedWidth(labelWidth1);
   HotPixelFilterRadius_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);
   HotPixelFilterRadius_Label.SetToolTip(hotPixelFilterRadiusToolTip);

   HotPixelFilterRadius_SpinBox.SetRange(int( TheSMHotPixelFilterRadiusParameter->MinimumValue()), int( TheSMHotPixelFilterRadiusParameter->MaximumValue()));
   HotPixelFilterRadius_SpinBox.SetToolTip(hotPixelFilterRadiusToolTip);
   HotPixelFilterRadius_SpinBox.SetFixedWidth(editWidth2);
   HotPixelFilterRadius_SpinBox.OnValueUpdated((SpinBox::value_event_handler) & StarMonitorInterface::__StarDetection_SpinValueUpdated, w);

   HotPixelFilterRadius_Sizer.SetSpacing(4);
   HotPixelFilterRadius_Sizer.Add(HotPixelFilterRadius_Label);
   HotPixelFilterRadius_Sizer.Add(HotPixelFilterRadius_SpinBox);
   HotPixelFilterRadius_Sizer.AddStretch();

   Sensitivity_NumericControl.label.SetText("Log(sensitivity):");
   Sensitivity_NumericControl.label.SetFixedWidth(labelWidth1);
   Sensitivity_NumericControl.slider.SetRange(-300, +300);
   Sensitivity_NumericControl.slider.SetMinWidth(200);
   Sensitivity_NumericControl.SetReal();
   Sensitivity_NumericControl.SetRange(Log(TheSMSensitivityParameter->MinimumValue()), Log(TheSMSensitivityParameter->MaximumValue()));
   Sensitivity_NumericControl.SetPrecision(2 /*TheSMSensitivityParameter->Precision()*/);
   Sensitivity_NumericControl.edit.SetFixedWidth(editWidth2);
   Sensitivity_NumericControl.SetToolTip("<p>Logarithm of the star detection sensitivity.</p>"
           "<p>The sensitivity of the star detection algorithm is measured with respect to the <i>local background</i> "
           "of each detected star. Given a star with estimated brightness <i>s</i> and local background <i>b</i>, "
           "sensitivity is the minimum value of (<i>s</i> &ndash; <i>b</i>)/<i>b</i> necessary to trigger star detection.</p>"
           "<p>Decrease this value to favor detection of fainter stars. Increase it to restrict detection to brighter stars, "
           "which may accelerate the star matching process, but at the risk of increasing the probability of failure. "
           "In general, you shouldn't need to change the default value of this parameter.</p>");
   Sensitivity_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__StarDetection_EditValueUpdated, w);

   PeakResponse_NumericControl.label.SetText("Peak response:");
   PeakResponse_NumericControl.label.SetFixedWidth(labelWidth1);
   PeakResponse_NumericControl.slider.SetRange(0, 100);
   PeakResponse_NumericControl.slider.SetMinWidth(200);
   PeakResponse_NumericControl.SetReal();
   PeakResponse_NumericControl.SetRange(TheSMPeakResponseParameter->MinimumValue(), TheSMPeakResponseParameter->MaximumValue());
   PeakResponse_NumericControl.SetPrecision(TheSMPeakResponseParameter->Precision());
   PeakResponse_NumericControl.edit.SetFixedWidth(editWidth2);
   PeakResponse_NumericControl.SetToolTip("<p>Star peak response.</p>"
           "<p>If you decrease this value, stars will need to have stronger (more prominent) peaks to be detected by "
           "the star detection algorithm. This is useful to prevent detection of saturated stars, as well as small "
           "nonstellar features. By increasing this parameter, the star detection algorithm will be more sensitive to "
           "<i>peakedness</i>, and hence more tolerant with relatively <i>flat</i> image features.</p>");
   PeakResponse_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__StarDetection_EditValueUpdated, w);

   MaxStarDistortion_NumericControl.label.SetText("Maximum distortion:");
   MaxStarDistortion_NumericControl.label.SetFixedWidth(labelWidth1);
   MaxStarDistortion_NumericControl.slider.SetRange(0, 100);
   MaxStarDistortion_NumericControl.slider.SetMinWidth(200);
   MaxStarDistortion_NumericControl.SetReal();
   MaxStarDistortion_NumericControl.SetRange(TheSMMaxStarDistortionParameter->MinimumValue(), TheSMMaxStarDistortionParameter->MaximumValue());
   MaxStarDistortion_NumericControl.SetPrecision(TheSMMaxStarDistortionParameter->Precision());
   MaxStarDistortion_NumericControl.edit.SetFixedWidth(editWidth2);
   MaxStarDistortion_NumericControl.SetToolTip("<p>Maximum star distortion.</p>"
           "<p>Star distortion is measured with respect to a perfect square, whose distortion is 1. Lower values mean "
           "more distortion. The distortion of a perfectly circular star is about 0.75 (actually, &pi;/4). Use this "
           "parameter, if necessary, to control inclusion of elongated stars, multiple stars, and nonstellar image "
           "features.</p>");
   MaxStarDistortion_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__StarDetection_EditValueUpdated, w);

   Invert_CheckBox.SetText("Inverted image");
   Invert_CheckBox.SetToolTip("<p>Invert the image prior to star detection.</p>"
           "<p>Select this option to register negative images (dark stars over a bright background).</p>");
   Invert_CheckBox.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   Invert_Sizer.AddSpacing(labelWidth1 + 4);
   Invert_Sizer.Add(Invert_CheckBox);
   Invert_Sizer.AddStretch();

   StarDetection_Sizer.SetSpacing(4);
   StarDetection_Sizer.Add(StructureLayers_Sizer);
   StarDetection_Sizer.Add(NoiseLayers_Sizer);
   StarDetection_Sizer.Add(HotPixelFilterRadius_Sizer);
   StarDetection_Sizer.Add(Sensitivity_NumericControl);
   StarDetection_Sizer.Add(PeakResponse_NumericControl);
   StarDetection_Sizer.Add(MaxStarDistortion_NumericControl);
   StarDetection_Sizer.Add(Invert_Sizer);

   StarDetection_Control.SetSizer(StarDetection_Sizer);
   StarDetection_Control.AdjustToContents();

   //

   Scale_SectionBar.SetTitle("Image Scale");
   Scale_SectionBar.SetSection(Scale_Control);
   //   Scale_SectionBar.OnToggleSection( (SectionBar::section_event_handler)&DynamicPSFInterface::__ToggleSection, w );

   const char* scaleModeTip = "<p>This parameter tells DynamicPSF how to compute the image scale to show "
           "full width at half maximum (FWHM) values:</p>"
           "<p><b>Pixels</b> will ignore image scale and show FWHM in pixel units. This is the default option.</p>"
           "<p><b>Standard FITS keywords</b> will try to read standard FOCALLEN, XPIXSZ and YPIXSZ FITS header "
           "keywords to compute the image scale in arcseconds per pixel.</p>"
           "<p><b>literal value</b> allows you to specify directly the image scale in arcseconds per pixel.</p>"
           "<p><b>Custom FITS keyword</b> allows you to specify the name of a custom FITS keyword whose value is "
           "the image scale in arcseconds per pixel.</p>";

   ScaleMode_Label.SetText("Scale mode:");
   ScaleMode_Label.SetToolTip(scaleModeTip);
   ScaleMode_Label.SetMinWidth(labelWidth1);
   ScaleMode_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);

   ScaleMode_ComboBox.AddItem("Pixels");
   ScaleMode_ComboBox.AddItem("Standard FITS keywords");
   ScaleMode_ComboBox.AddItem("Literal value");
   //ScaleMode_ComboBox.AddItem( "Custom FITS keyword" );
   ScaleMode_ComboBox.SetToolTip(scaleModeTip);
   ScaleMode_ComboBox.OnItemSelected((ComboBox::item_event_handler) & StarMonitorInterface::__ItemSelected, w);

   ScaleMode_Sizer.SetSpacing(4);
   ScaleMode_Sizer.Add(ScaleMode_Label);
   ScaleMode_Sizer.Add(ScaleMode_ComboBox);
   ScaleMode_Sizer.AddStretch();

   const char* scaleValueTip = "<p>When <i>scale mode</i> has been set to <i>literal value</i>, this is the image "
           "scale in arcseconds per pixel.</p>";

   ScaleValue_NumericEdit.label.SetText("Image scale (arcsec/px):");
   ScaleValue_NumericEdit.label.SetFixedWidth(labelWidth1);
   ScaleValue_NumericEdit.SetReal();
   ScaleValue_NumericEdit.SetRange(TheSMScaleValueParameter->MinimumValue(), TheSMScaleValueParameter->MaximumValue());
   ScaleValue_NumericEdit.SetPrecision(TheSMScaleValueParameter->Precision());
   ScaleValue_NumericEdit.sizer.AddStretch();
   ScaleValue_NumericEdit.SetToolTip(scaleValueTip);
   ScaleValue_NumericEdit.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__NumericEdit_ValueUpdated, w);

   Scale_Sizer.SetSpacing(4);
   Scale_Sizer.Add(ScaleMode_Sizer);
   Scale_Sizer.Add(ScaleValue_NumericEdit);

   Scale_Control.SetSizer(Scale_Sizer);

   // Alert Section -----------------------------------------

   Alert_SectionBar.SetTitle("Alert");
   Alert_SectionBar.SetSection(Alert_Control);
   Alert_SectionBar.EnableTitleCheckBox();
   Alert_SectionBar.OnCheck((SectionBar::check_event_handler) & StarMonitorInterface::__CheckSection, w);

   //
   Log_TextBox.SetText("execute log");
   //Log_TextBox.SetMinSize(50, 50);
   Log_TextBox.SetReadOnly();
   StdIn_Edit.OnEditCompleted((Edit::edit_event_handler) & StarMonitorInterface::__EditCompleted, w);
   StdIn_Edit.Disable(); //disable while external processing is not run
   //

   AlertExecute_Label.SetText("Execute:");
   AlertExecute_Label.SetMinWidth(labelWidth1);
   AlertExecute_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);

   AlertExecute_Edit.OnEditCompleted((Edit::edit_event_handler) & StarMonitorInterface::__EditCompleted, w);
#ifdef __PCL_LINUX
   AlertExecute_Edit.SetToolTip(""); //ToDo
#endif
#ifdef __PCL_FREEBSD
   AlertExecute_Edit.SetToolTip(""); //ToDo
#endif
#ifdef __PCL_MACOSX
   AlertExecute_Edit.SetToolTip(""); //ToDo
#endif
#ifdef __PCL_WINDOWS
   AlertExecute_Edit.SetToolTip("Example:<br><b>C:\\WINDOWS\\system32\\mplay32.exe</b>");
#endif


   AlertExecute_Sizer.SetSpacing(4);
   AlertExecute_Sizer.Add(AlertExecute_Label);
   AlertExecute_Sizer.Add(AlertExecute_Edit, 100);

   //

   AlertArguments_Label.SetText("Arguments:");
   AlertArguments_Label.SetMinWidth(labelWidth1);
   AlertArguments_Label.SetTextAlignment(TextAlign::Right | TextAlign::VertCenter);

   AlertArguments_Edit.OnEditCompleted((Edit::edit_event_handler) & StarMonitorInterface::__EditCompleted, w);

   AlertArgumentsTest_ToolButton.SetIcon(Bitmap(String(":/images/image_container/launch_item.png")));
   AlertArgumentsTest_ToolButton.SetFixedSize(19, 19);
   AlertArgumentsTest_ToolButton.SetToolTip("<p>Click to test Alert Command</p>");
   AlertArgumentsTest_ToolButton.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   AlertArguments_Sizer.SetSpacing(4);
   AlertArguments_Sizer.Add(AlertArguments_Label);
   AlertArguments_Sizer.Add(AlertArguments_Edit, 100);
   AlertArguments_Sizer.Add(AlertArgumentsTest_ToolButton);

   //

   const char* alertTip = "<p>Generate the alert if observed %s value %s.</p>";
   const char* alertTipCheck = "enable/disable ";

   const String alertFWHMxTip = String().Format(alertTip, "FWHMx(pixels)", "greater");
   AlertFWHMx_CheckBox.SetToolTip(String(alertTipCheck) + alertFWHMxTip);
   AlertFWHMx_CheckBox.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);
   AlertFWHMx_NumericControl.sizer.Insert(1, AlertFWHMx_CheckBox);
   AlertFWHMx_NumericControl.SetToolTip(alertFWHMxTip);
   AlertFWHMx_NumericControl.label.SetText("FWHMx:");
   AlertFWHMx_NumericControl.label.SetFixedWidth(labelWidth1);
   AlertFWHMx_NumericControl.edit.SetFixedWidth(editWidth2);
   AlertFWHMx_NumericControl.slider.SetMinWidth(200);
   AlertFWHMx_NumericControl.SetRange(TheSMAlertFWHMxParameter->MinimumValue(), TheSMAlertFWHMxParameter->MaximumValue());
   AlertFWHMx_NumericControl.SetPrecision(TheSMAlertFWHMxParameter->Precision());
   AlertFWHMx_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__NumericEdit_ValueUpdated, w);

   //

   const String alertRoundnesTip = String().Format(alertTip, "roundness", "less");
   AlertRoundness_CheckBox.SetToolTip(String(alertTipCheck) + alertRoundnesTip);
   AlertRoundness_CheckBox.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);
   AlertRoundness_NumericControl.sizer.Insert(1, AlertRoundness_CheckBox);
   AlertRoundness_NumericControl.SetToolTip(alertRoundnesTip);
   AlertRoundness_NumericControl.label.SetText("Roundness:");
   AlertRoundness_NumericControl.label.SetFixedWidth(labelWidth1);
   AlertRoundness_NumericControl.edit.SetFixedWidth(editWidth2);
   AlertRoundness_NumericControl.slider.SetMinWidth(200);
   AlertRoundness_NumericControl.slider.SetRange(0, 200);
   AlertRoundness_NumericControl.SetRange(TheSMAlertRoundnessParameter->MinimumValue(), TheSMAlertRoundnessParameter->MaximumValue());
   AlertRoundness_NumericControl.SetPrecision(TheSMAlertRoundnessParameter->Precision());
   AlertRoundness_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__NumericEdit_ValueUpdated, w);

   //

   const String alertBackgroundTip = String().Format(alertTip, "Background", "brighter");
   AlertBackground_CheckBox.SetToolTip(String(alertTipCheck) + alertBackgroundTip);
   AlertBackground_CheckBox.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);
   AlertBackground_NumericControl.sizer.Insert(1, AlertBackground_CheckBox);
   AlertBackground_NumericControl.SetToolTip(alertBackgroundTip);
   AlertBackground_NumericControl.label.SetText("Background:");
   AlertBackground_NumericControl.label.SetFixedWidth(labelWidth1);
   AlertBackground_NumericControl.edit.SetFixedWidth(editWidth2);
   AlertBackground_NumericControl.slider.SetMinWidth(200);
   AlertBackground_NumericControl.slider.SetRange(0, 0x7fffffff);
   AlertBackground_NumericControl.SetRange(TheSMAlertBackgroundParameter->MinimumValue(), TheSMAlertBackgroundParameter->MaximumValue());
   AlertBackground_NumericControl.SetPrecision(TheSMAlertBackgroundParameter->Precision());
   AlertBackground_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__NumericEdit_ValueUpdated, w);

   //

   const String alertStarQuantityTip = String().Format(alertTip, "StarQuantity", "less");
   AlertStarQuantity_CheckBox.SetToolTip(String(alertTipCheck) + alertStarQuantityTip);
   AlertStarQuantity_CheckBox.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);
   AlertStarQuantity_NumericControl.sizer.Insert(1, AlertStarQuantity_CheckBox);
   AlertStarQuantity_NumericControl.SetInteger();
   AlertStarQuantity_NumericControl.SetToolTip(alertStarQuantityTip);
   AlertStarQuantity_NumericControl.label.SetText("StarQuantity:");
   AlertStarQuantity_NumericControl.label.SetFixedWidth(labelWidth1);
   AlertStarQuantity_NumericControl.edit.SetFixedWidth(editWidth2);
   AlertStarQuantity_NumericControl.slider.SetMinWidth(200);
   AlertStarQuantity_NumericControl.SetRange(TheSMAlertStarQuantityParameter->MinimumValue(), TheSMAlertStarQuantityParameter->MaximumValue());
   AlertStarQuantity_NumericControl.OnValueUpdated((NumericEdit::value_event_handler) & StarMonitorInterface::__NumericEdit_ValueUpdated, w);

   //

   Alert_Sizer.SetSpacing(4);
   Alert_Sizer.Add(Log_TextBox,100);
   Alert_Sizer.Add(StdIn_Edit);
   Alert_Sizer.Add(AlertExecute_Sizer);
   Alert_Sizer.Add(AlertArguments_Sizer);
   Alert_Sizer.Add(AlertFWHMx_NumericControl);
   Alert_Sizer.Add(AlertRoundness_NumericControl);
   Alert_Sizer.Add(AlertBackground_NumericControl);
   Alert_Sizer.Add(AlertStarQuantity_NumericControl);

   Alert_Control.SetSizer(Alert_Sizer);


   // Show 3D -------------------------------------------

   Show3D_SectionBar.SetTitle("Show 3D");
   Show3D_SectionBar.SetSection(Show3D_Control);

   Show3D_Button.SetIcon(Bitmap(String(":/images/project.png")));
   Show3D_Button.SetText("Start");
   Show3D_Button.OnClick((Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   ShowVoronoi_CheckBox.SetText("Show Voronoi");
   ShowVoronoi_CheckBox.OnClick((pcl::Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   ShowDelaunay_CheckBox.SetText("Show Delaunay");
   ShowDelaunay_CheckBox.OnClick((pcl::Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   Flat_CheckBox.SetText("Flat Delaunay");
   Flat_CheckBox.OnClick((pcl::Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   MoveAll_CheckBox.SetText("Move All(in demo mode only)");
   MoveAll_CheckBox.OnClick((pcl::Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   CameraFixZ_CheckBox.SetText("Camera fix Z");
   CameraFixZ_CheckBox.OnClick((pcl::Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);

   CameraFixXY_CheckBox.SetText("Camera fix XY");
   CameraFixXY_CheckBox.OnClick((pcl::Button::click_event_handler) & StarMonitorInterface::__Button_Click, w);


   Show3D_Sizer.SetSpacing(4);
   Show3D_Sizer.Add(Show3D_Button);
   Show3D_Sizer.Add(ShowVoronoi_CheckBox);
   Show3D_Sizer.Add(ShowDelaunay_CheckBox);
   Show3D_Sizer.Add(Flat_CheckBox);
   Show3D_Sizer.Add(MoveAll_CheckBox);
   Show3D_Sizer.Add(CameraFixZ_CheckBox);
   Show3D_Sizer.Add(CameraFixXY_CheckBox);

   Show3D_Control.SetSizer(Show3D_Sizer);

   // Global Sizer --------------------------------------------

   Global_Sizer.SetMargin(8);
   Global_Sizer.SetSpacing(6);

   Global_Sizer.Add(TargetImages_TreeBox, 100);

   Global_Sizer.Add(GeneralParameters_Sizer);

   Global_Sizer.Add(FormatHints_SectionBar);
   Global_Sizer.Add(FormatHints_Control);

   Global_Sizer.Add(StarDetection_SectionBar);
   Global_Sizer.Add(StarDetection_Control);
   Global_Sizer.Add(Scale_SectionBar);
   Global_Sizer.Add(Scale_Control);
   Global_Sizer.Add(Alert_SectionBar);
   Global_Sizer.Add(Alert_Control);
   Global_Sizer.Add(Show3D_SectionBar);
   Global_Sizer.Add(Show3D_Control);
   w.SetMinWidth();

   StarDetection_Control.Hide();
   FormatHints_Control.Hide();
   Scale_Control.Hide();
   Alert_Control.Hide();
   Show3D_Control.Hide();

   w.SetSizer(Global_Sizer);
   //w.AdjustToContents();
   //w.SetMinWidth(AlertStarQuantity_NumericControl.Width());

   // ----------------------------------------------------------------------------
   UpdateRealTime_Timer.SetSingleShot();
   UpdateRealTime_Timer.SetInterval(0.1);
   UpdateRealTime_Timer.OnTimer((Timer::timer_event_handler) & StarMonitorInterface::__Timer, w);
   
   my_AlertProcess.OnStarted((ExternalProcess::process_event_handler) & StarMonitorInterface::__ExternalProcessOnStarted, w);
   my_AlertProcess.OnFinished((ExternalProcess::process_exit_event_handler) & StarMonitorInterface::__ExternalProcessOnFinished, w);
   my_AlertProcess.OnStandardOutputDataAvailable((ExternalProcess::process_event_handler) & StarMonitorInterface::__ExternalProcessOnStandardOutputDataAvailable, w);
   my_AlertProcess.OnStandardErrorDataAvailable((ExternalProcess::process_event_handler) & StarMonitorInterface::__ExternalProcessOnStandardOutputDataAvailable, w);
   my_AlertProcess.OnError((ExternalProcess::process_error_event_handler) & StarMonitorInterface::__ExternalProcessOnError, w);

}

// ----------------------------------------------------------------------------

} // pcl

// ****************************************************************************
// EOF StarMonitorInterface.cpp - Released 2014/01/30 00:00:00 UTC
