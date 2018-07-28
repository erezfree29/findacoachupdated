class Deletefromtrainer < ActiveRecord::Migration[5.2]
  def change
    remove_column :trainers, :latitude
    remove_column :trainers, :longtitude
    add_column :trainers, :longitude, :float
    add_column :trainers, :latitude, :float
  end
end
