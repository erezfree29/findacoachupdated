class AddcordiantestoTrainers < ActiveRecord::Migration[5.2]
  def change
    add_column :trainers, :latitude, :float
  end
end
