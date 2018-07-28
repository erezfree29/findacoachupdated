class AddanodthercordiantesTrainers < ActiveRecord::Migration[5.2]
  def change
    add_column :trainers, :longtitude, :float
  end
end
