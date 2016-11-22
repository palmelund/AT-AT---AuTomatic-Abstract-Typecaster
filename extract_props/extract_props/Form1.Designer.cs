namespace extract_props
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.pictureBoxOriginal = new System.Windows.Forms.PictureBox();
            this.pictureBoxCleaned = new System.Windows.Forms.PictureBox();
            this.buttonLoadIgnores = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.buttonNextImage = new System.Windows.Forms.Button();
            this.progressBar2 = new System.Windows.Forms.ProgressBar();
            this.buttonLoadFiles = new System.Windows.Forms.Button();
            this.progressBar3 = new System.Windows.Forms.ProgressBar();
            this.labelFilesLoaded = new System.Windows.Forms.Label();
            this.labelCurrentImage = new System.Windows.Forms.Label();
            this.labelwhitepixels = new System.Windows.Forms.Label();
            this.labelblackpixels = new System.Windows.Forms.Label();
            this.labeldifference = new System.Windows.Forms.Label();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.checkBoxbackground = new System.Windows.Forms.CheckBox();
            this.pictureBoxBgndOrig = new System.Windows.Forms.PictureBox();
            this.pictureBoxBgnAfter = new System.Windows.Forms.PictureBox();
            this.labelIgnoreColors = new System.Windows.Forms.Label();
            this.pictureBoxBackgroundCleaned = new System.Windows.Forms.PictureBox();
            this.buttonGenFromFile = new System.Windows.Forms.Button();
            this.buttonArff = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxOriginal)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCleaned)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxBgndOrig)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxBgnAfter)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxBackgroundCleaned)).BeginInit();
            this.SuspendLayout();
            // 
            // pictureBoxOriginal
            // 
            this.pictureBoxOriginal.Location = new System.Drawing.Point(12, 12);
            this.pictureBoxOriginal.Name = "pictureBoxOriginal";
            this.pictureBoxOriginal.Size = new System.Drawing.Size(400, 200);
            this.pictureBoxOriginal.TabIndex = 0;
            this.pictureBoxOriginal.TabStop = false;
            // 
            // pictureBoxCleaned
            // 
            this.pictureBoxCleaned.Location = new System.Drawing.Point(12, 337);
            this.pictureBoxCleaned.Name = "pictureBoxCleaned";
            this.pictureBoxCleaned.Size = new System.Drawing.Size(400, 200);
            this.pictureBoxCleaned.TabIndex = 1;
            this.pictureBoxCleaned.TabStop = false;
            // 
            // buttonLoadIgnores
            // 
            this.buttonLoadIgnores.Location = new System.Drawing.Point(12, 247);
            this.buttonLoadIgnores.Name = "buttonLoadIgnores";
            this.buttonLoadIgnores.Size = new System.Drawing.Size(75, 23);
            this.buttonLoadIgnores.TabIndex = 2;
            this.buttonLoadIgnores.Text = "load Ignores";
            this.buttonLoadIgnores.UseVisualStyleBackColor = true;
            this.buttonLoadIgnores.Click += new System.EventHandler(this.buttonLoadIgnores_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(93, 247);
            this.progressBar1.Maximum = 1156;
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(100, 23);
            this.progressBar1.TabIndex = 3;
            // 
            // buttonNextImage
            // 
            this.buttonNextImage.Location = new System.Drawing.Point(12, 276);
            this.buttonNextImage.Name = "buttonNextImage";
            this.buttonNextImage.Size = new System.Drawing.Size(75, 23);
            this.buttonNextImage.TabIndex = 4;
            this.buttonNextImage.Text = "Next Image";
            this.buttonNextImage.UseVisualStyleBackColor = true;
            this.buttonNextImage.Click += new System.EventHandler(this.buttonNextImage_Click);
            // 
            // progressBar2
            // 
            this.progressBar2.Location = new System.Drawing.Point(93, 276);
            this.progressBar2.Maximum = 80000;
            this.progressBar2.Name = "progressBar2";
            this.progressBar2.Size = new System.Drawing.Size(100, 23);
            this.progressBar2.TabIndex = 5;
            // 
            // buttonLoadFiles
            // 
            this.buttonLoadFiles.Location = new System.Drawing.Point(12, 218);
            this.buttonLoadFiles.Name = "buttonLoadFiles";
            this.buttonLoadFiles.Size = new System.Drawing.Size(75, 23);
            this.buttonLoadFiles.TabIndex = 6;
            this.buttonLoadFiles.Text = "Load Files";
            this.buttonLoadFiles.UseVisualStyleBackColor = true;
            this.buttonLoadFiles.Click += new System.EventHandler(this.buttonLoadFiles_Click);
            // 
            // progressBar3
            // 
            this.progressBar3.Location = new System.Drawing.Point(93, 218);
            this.progressBar3.Name = "progressBar3";
            this.progressBar3.Size = new System.Drawing.Size(100, 23);
            this.progressBar3.TabIndex = 7;
            // 
            // labelFilesLoaded
            // 
            this.labelFilesLoaded.AutoSize = true;
            this.labelFilesLoaded.Location = new System.Drawing.Point(199, 223);
            this.labelFilesLoaded.Name = "labelFilesLoaded";
            this.labelFilesLoaded.Size = new System.Drawing.Size(0, 13);
            this.labelFilesLoaded.TabIndex = 8;
            // 
            // labelCurrentImage
            // 
            this.labelCurrentImage.AutoSize = true;
            this.labelCurrentImage.Location = new System.Drawing.Point(199, 281);
            this.labelCurrentImage.Name = "labelCurrentImage";
            this.labelCurrentImage.Size = new System.Drawing.Size(0, 13);
            this.labelCurrentImage.TabIndex = 9;
            // 
            // labelwhitepixels
            // 
            this.labelwhitepixels.AutoSize = true;
            this.labelwhitepixels.Location = new System.Drawing.Point(9, 540);
            this.labelwhitepixels.Name = "labelwhitepixels";
            this.labelwhitepixels.Size = new System.Drawing.Size(35, 13);
            this.labelwhitepixels.TabIndex = 10;
            this.labelwhitepixels.Text = "label1";
            // 
            // labelblackpixels
            // 
            this.labelblackpixels.AutoSize = true;
            this.labelblackpixels.Location = new System.Drawing.Point(9, 559);
            this.labelblackpixels.Name = "labelblackpixels";
            this.labelblackpixels.Size = new System.Drawing.Size(35, 13);
            this.labelblackpixels.TabIndex = 11;
            this.labelblackpixels.Text = "label1";
            // 
            // labeldifference
            // 
            this.labeldifference.AutoSize = true;
            this.labeldifference.Location = new System.Drawing.Point(9, 580);
            this.labeldifference.Name = "labeldifference";
            this.labeldifference.Size = new System.Drawing.Size(35, 13);
            this.labeldifference.TabIndex = 12;
            this.labeldifference.Text = "label1";
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(202, 593);
            this.textBox1.Multiline = true;
            this.textBox1.Name = "textBox1";
            this.textBox1.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
            this.textBox1.Size = new System.Drawing.Size(236, 56);
            this.textBox1.TabIndex = 13;
            // 
            // checkBoxbackground
            // 
            this.checkBoxbackground.AutoSize = true;
            this.checkBoxbackground.Location = new System.Drawing.Point(649, 12);
            this.checkBoxbackground.Name = "checkBoxbackground";
            this.checkBoxbackground.Size = new System.Drawing.Size(110, 17);
            this.checkBoxbackground.TabIndex = 14;
            this.checkBoxbackground.Text = "Backgroundmode";
            this.checkBoxbackground.UseVisualStyleBackColor = true;
            // 
            // pictureBoxBgndOrig
            // 
            this.pictureBoxBgndOrig.Location = new System.Drawing.Point(765, 12);
            this.pictureBoxBgndOrig.Name = "pictureBoxBgndOrig";
            this.pictureBoxBgndOrig.Size = new System.Drawing.Size(400, 200);
            this.pictureBoxBgndOrig.TabIndex = 15;
            this.pictureBoxBgndOrig.TabStop = false;
            // 
            // pictureBoxBgnAfter
            // 
            this.pictureBoxBgnAfter.Location = new System.Drawing.Point(765, 218);
            this.pictureBoxBgnAfter.Name = "pictureBoxBgnAfter";
            this.pictureBoxBgnAfter.Size = new System.Drawing.Size(400, 200);
            this.pictureBoxBgnAfter.TabIndex = 16;
            this.pictureBoxBgnAfter.TabStop = false;
            // 
            // labelIgnoreColors
            // 
            this.labelIgnoreColors.AutoSize = true;
            this.labelIgnoreColors.Location = new System.Drawing.Point(428, 12);
            this.labelIgnoreColors.Name = "labelIgnoreColors";
            this.labelIgnoreColors.Size = new System.Drawing.Size(134, 13);
            this.labelIgnoreColors.TabIndex = 17;
            this.labelIgnoreColors.Text = "Background colors loaded:";
            // 
            // pictureBoxBackgroundCleaned
            // 
            this.pictureBoxBackgroundCleaned.Location = new System.Drawing.Point(765, 424);
            this.pictureBoxBackgroundCleaned.Name = "pictureBoxBackgroundCleaned";
            this.pictureBoxBackgroundCleaned.Size = new System.Drawing.Size(400, 200);
            this.pictureBoxBackgroundCleaned.TabIndex = 18;
            this.pictureBoxBackgroundCleaned.TabStop = false;
            // 
            // buttonGenFromFile
            // 
            this.buttonGenFromFile.Location = new System.Drawing.Point(444, 626);
            this.buttonGenFromFile.Name = "buttonGenFromFile";
            this.buttonGenFromFile.Size = new System.Drawing.Size(86, 23);
            this.buttonGenFromFile.TabIndex = 19;
            this.buttonGenFromFile.Text = "DONT Push !!";
            this.buttonGenFromFile.UseVisualStyleBackColor = true;
            this.buttonGenFromFile.Click += new System.EventHandler(this.buttonGenFromFile_Click);
            // 
            // buttonArff
            // 
            this.buttonArff.Location = new System.Drawing.Point(570, 626);
            this.buttonArff.Name = "buttonArff";
            this.buttonArff.Size = new System.Drawing.Size(75, 23);
            this.buttonArff.TabIndex = 20;
            this.buttonArff.Text = "Arff File";
            this.buttonArff.UseVisualStyleBackColor = true;
            this.buttonArff.Click += new System.EventHandler(this.buttonArff_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1177, 653);
            this.Controls.Add(this.buttonArff);
            this.Controls.Add(this.buttonGenFromFile);
            this.Controls.Add(this.pictureBoxBackgroundCleaned);
            this.Controls.Add(this.labelIgnoreColors);
            this.Controls.Add(this.pictureBoxBgnAfter);
            this.Controls.Add(this.pictureBoxBgndOrig);
            this.Controls.Add(this.checkBoxbackground);
            this.Controls.Add(this.textBox1);
            this.Controls.Add(this.labeldifference);
            this.Controls.Add(this.labelblackpixels);
            this.Controls.Add(this.labelwhitepixels);
            this.Controls.Add(this.labelCurrentImage);
            this.Controls.Add(this.labelFilesLoaded);
            this.Controls.Add(this.progressBar3);
            this.Controls.Add(this.buttonLoadFiles);
            this.Controls.Add(this.progressBar2);
            this.Controls.Add(this.buttonNextImage);
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.buttonLoadIgnores);
            this.Controls.Add(this.pictureBoxCleaned);
            this.Controls.Add(this.pictureBoxOriginal);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxOriginal)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxCleaned)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxBgndOrig)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxBgnAfter)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBoxBackgroundCleaned)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox pictureBoxOriginal;
        private System.Windows.Forms.PictureBox pictureBoxCleaned;
        private System.Windows.Forms.Button buttonLoadIgnores;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button buttonNextImage;
        private System.Windows.Forms.ProgressBar progressBar2;
        private System.Windows.Forms.Button buttonLoadFiles;
        private System.Windows.Forms.ProgressBar progressBar3;
        private System.Windows.Forms.Label labelFilesLoaded;
        private System.Windows.Forms.Label labelCurrentImage;
        private System.Windows.Forms.Label labelwhitepixels;
        private System.Windows.Forms.Label labelblackpixels;
        private System.Windows.Forms.Label labeldifference;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.CheckBox checkBoxbackground;
        private System.Windows.Forms.PictureBox pictureBoxBgndOrig;
        private System.Windows.Forms.PictureBox pictureBoxBgnAfter;
        private System.Windows.Forms.Label labelIgnoreColors;
        private System.Windows.Forms.PictureBox pictureBoxBackgroundCleaned;
        private System.Windows.Forms.Button buttonGenFromFile;
        private System.Windows.Forms.Button buttonArff;
    }
}

